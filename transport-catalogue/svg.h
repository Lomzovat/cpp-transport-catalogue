#pragma once
#define _USE_MATH_DEFINES

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <cmath>
#include <variant>


using namespace std::literals;

namespace svg {

    class Rgb {
    public:
        Rgb() = default;
        Rgb(uint8_t red, uint8_t green, uint8_t blue);

        uint8_t red_ = 0;
        uint8_t green_ = 0;
        uint8_t blue_ = 0;
    };
    inline void PrintColor(std::ostream& out, Rgb& rgb);

    class Rgba {
    public:
        Rgba() = default;
        Rgba(uint8_t red, uint8_t green, uint8_t blue, double opacity);

        uint8_t red_ = 0;
        uint8_t green_ = 0;
        uint8_t blue_ = 0;
        double opacity_ = 1.0;
    };
    inline void PrintColor(std::ostream& out, Rgba& rgba);

    using Color = std::variant<std::monostate, std::string, Rgb, Rgba>;
    inline const Color NoneColor{ "none" };

    inline void PrintColor(std::ostream& out, std::monostate);
    inline void PrintColor(std::ostream& out, std::string& color);
    std::ostream& operator<<(std::ostream& out, const Color& color);

    enum class StrokeLineCap {
        BUTT,
        ROUND,
        SQUARE,
    };

    inline std::ostream& operator<<(std::ostream& out, StrokeLineCap stroke_line_cap) {

        if (stroke_line_cap == StrokeLineCap::BUTT) {
            out << "butt"sv;
        }
        else if (stroke_line_cap == StrokeLineCap::ROUND) {
            out << "round"sv;
        }
        else if (stroke_line_cap == StrokeLineCap::SQUARE) {
            out << "square"sv;
        }
        return out;
    }

    enum class StrokeLineJoin {
        ARCS,
        BEVEL,
        MITER,
        MITER_CLIP,
        ROUND,
    };

    inline std::ostream& operator<<(std::ostream& out, StrokeLineJoin stroke_line_join) {

        if (stroke_line_join == StrokeLineJoin::ARCS) {
            out << "arcs"sv;
        }
        else if (stroke_line_join == StrokeLineJoin::BEVEL) {
            out << "bevel"sv;
        }
        else if (stroke_line_join == StrokeLineJoin::MITER) {
            out << "miter"sv;
        }
        else if (stroke_line_join == StrokeLineJoin::MITER_CLIP) {
            out << "miter-clip"sv;
        }
        else if (stroke_line_join == StrokeLineJoin::ROUND) {
            out << "round"sv;
        }
        return out;
    }

    template<typename Owner>
    class PathProps {
    public:
        Owner& SetFillColor(const Color& color) {
            fill_color_ = std::move(color);
            return AsOwner();
        }

        Owner& SetStrokeColor(const Color& color) {
            stroke_color_ = std::move(color);
            return AsOwner();
        }

        Owner& SetStrokeWidth(double width) {
            stroke_width_ = width;
            return AsOwner();
        }

        Owner& SetStrokeLineCap(StrokeLineCap line_cap) {
            stroke_linecap_ = line_cap;
            return AsOwner();
        }

        Owner& SetStrokeLineJoin(StrokeLineJoin line_join) {
            stroke_linejoin_ = line_join;
            return AsOwner();
        }

    protected:
        ~PathProps() = default;

        void RenderAttrs(std::ostream& out) const {


            if (fill_color_ != std::nullopt) {
                out << "fill=\""sv << *fill_color_ << "\" "sv;
            }
            if (stroke_color_ != std::nullopt) {
                out << "stroke=\""sv << *stroke_color_ << "\" "sv;
            }
            if (stroke_width_ != std::nullopt) {
                out << "stroke-width=\""sv << *stroke_width_ << "\" "sv;
            }
            if (stroke_linecap_ != std::nullopt) {
                out << "stroke-linecap=\""sv << *stroke_linecap_ << "\" "sv;
            }
            if (stroke_linejoin_ != std::nullopt) {
                out << "stroke-linejoin=\""sv << *stroke_linejoin_ << "\" "sv;
            }
        }

    private:
        std::optional<Color> fill_color_;
        std::optional<Color> stroke_color_;
        std::optional<double> stroke_width_;
        std::optional<StrokeLineCap> stroke_linecap_;
        std::optional<StrokeLineJoin> stroke_linejoin_;

        Owner& AsOwner() {
            return static_cast<Owner&>(*this);
        }
    };

    struct Point {
        Point() = default;
        Point(double x, double y) :
            x(x),
            y(y)
        {}

        double x = 0;
        double y = 0;
    };

    struct RenderContext {
        RenderContext(std::ostream& out);
        RenderContext(std::ostream& out, int indent_step, int indent = 0) : out_(out)
            , indent_step_(indent_step)
            , indent_(indent) {}
        RenderContext Indented() const;
        void RenderIndent() const;

        std::ostream& out_;
        int indent_step_ = 0;
        int indent_ = 0;
    };

    class Object {
    public:
        void Render(const RenderContext& context) const;
        virtual ~Object() = default;

    private:
        virtual void RenderObject(const RenderContext& context) const = 0;
    };

    class Circle final : public Object, public PathProps<Circle> {
    public:
        Circle& SetCenter(Point center);
        Circle& SetRadius(double radius);

    private:
        Point center_;
        double radius_ = 1.0;

        void RenderObject(const RenderContext& context) const override;
    };

    class Polyline final : public Object, public PathProps<Polyline> {
    public:
        Polyline& AddPoint(Point point);

    private:
        std::vector<Point> points_;
        void RenderObject(const RenderContext& context) const override;

    };

    class Text final : public Object, public PathProps<Text> {
    public:
        Text& SetPosition(Point pos);
        Text& SetOffset(Point offset);
        Text& SetFontSize(uint32_t size);
        Text& SetFontFamily(std::string font_family);
        Text& SetFontWeight(std::string font_weight);
        Text& SetData(std::string data);

    private:
        Point position_ = { 0.0, 0.0 };
        Point offset_ = { 0.0, 0.0 };
        uint32_t font_size_ = 1;
        std::string font_family_;
        std::string font_weight_;
        std::string data_;

        static std::string DeleteSpaces(const std::string& str);
        static std::string UniqueSymbols(const std::string& str);

        void RenderObject(const RenderContext& context) const override;
    };

    class ObjectContainer {
    public:
        virtual ~ObjectContainer() = default;

        template<typename Obj>
        void Add(Obj obj);

        virtual void AddPtr(std::unique_ptr<Object>&&) = 0;

    protected:
        std::vector<std::unique_ptr<Object>> objects_;
    };

    template<typename Obj>
    void ObjectContainer::Add(Obj obj) {
        objects_.emplace_back(std::make_unique<Obj>(std::move(obj)));
    }

    class Drawable {
    public:
        virtual void Draw(ObjectContainer& container) const = 0;
        virtual ~Drawable() = default;
    };

    class Document : public ObjectContainer {
    public:

        void AddPtr(std::unique_ptr<Object>&& obj) override {
            objects_.emplace_back(std::move(obj));
        }

        void Render(std::ostream& out) const;

    };

}//end namespace svg
