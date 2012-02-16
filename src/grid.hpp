#ifndef CRUST_GRID_HPP
#define CRUST_GRID_HPP

#include "int_geometry.hpp"

#include <algorithm>
#include <cmath>

namespace crust {
    template <typename T>
    class Grid {
    public:
        typedef T Element;

        explicit Grid(Element const &defaultValue = Element()) :
            defaultValue_(defaultValue),
            elements_(0),
            normalized_(true)
        { }
        
        ~Grid()
        {
            delete[] elements_;
        }

        int getX() const
        {
            return innerBox_.p1.x;
        }

        int getY() const
        {
            return innerBox_.p1.y;
        }

        int getWidth() const
        {
            return innerBox_.getWidth();
        }

        int getHeight() const
        {
            return innerBox_.getHeight();
        }

        bool isEmpty() const
        {
            return innerBox_.isEmpty();
        }
        
        int getPitch() const
        {
            return outerBox_.getWidth();
        }

        bool isNormalized() const
        {
            return normalized_;
        }

        Element const &getElement(int x, int y) const
        {
            if (innerBox_.containsPoint(IntVector2(x, y))) {
                return elements_[getIndex(x, y)];
            } else {
                return defaultValue_;
            }
        }

        void setElement(int x, int y, Element const &value)
        {
            if (value != defaultValue_) {
                addElement(x, y, value);
            } else {
                removeElement(x, y);
            }
        }

        void swap(Grid &other)
        {
            std::swap(innerBox_, other.innerBox_);
            std::swap(outerBox_, other.outerBox_);
            std::swap(defaultValue_, other.defaultValue_);
            std::swap(elements_, other.elements_);
            std::swap(normalized_, other.normalized_);
        }
        
        void normalize()
        {
            if (!normalized_) {
                IntBox2 box;
                for (int y = innerBox_.p1.y; y < innerBox_.p2.y; ++y) {
                    for (int x = innerBox_.p1.x; x < innerBox_.p2.x; ++x) {
                        if (elements_[getIndex(x, y)] != defaultValue_) {
                            box.mergePoint(IntVector2(x, y));
                        }
                    }
                }
                innerBox_ = box;
                normalized_ = true;
            }
        }

    private:
        explicit Grid(IntBox2 const &box, Element const &defaultValue) :
            innerBox_(box),
            outerBox_(box),
            defaultValue_(defaultValue),
            elements_(0),
            normalized_(true)
        {
            int dx = std::max(1, int(0.5f * M_SQRT2 * float(box.getWidth()) + 0.5f));
            int dy = std::max(1, int(0.5f * M_SQRT2 * float(box.getHeight()) + 0.5f));
            outerBox_.pad(IntVector2(dx, dy));
            elements_ = new Element[outerBox_.getArea()];
            std::fill(elements_, elements_ + outerBox_.getArea(), defaultValue);
        }

        // Noncopyable.
        //
        // TODO: Public implementations.
        Grid(Grid const &other);
        Grid &operator=(Grid const &other);

        IntBox2 innerBox_;
        IntBox2 outerBox_;
        Element defaultValue_;
        Element *elements_;
        bool normalized_;

        void addElement(int x, int y, Element const &value)
        {
            if (innerBox_.containsPoint(IntVector2(x, y))) {
                elements_[getIndex(x, y)] = value;
            } else if (outerBox_.containsPoint(IntVector2(x, y))) {
                innerBox_.mergePoint(IntVector2(x, y));
                elements_[getIndex(x, y)] = value;
            } else {
                normalize();
                
                IntBox2 box(innerBox_);
                box.mergePoint(IntVector2(x, y));
                
                Grid other(box, defaultValue_);
                copyElements(other);
                swap(other);
                
                elements_[getIndex(x, y)] = value;
            }
        }

        void removeElement(int x, int y)
        {
            if (innerBox_.containsPoint(IntVector2(x, y))) {
                elements_[getIndex(x, y)] = defaultValue_;
                normalized_ = false;
            }
        }

        int getIndex(int x, int y) const
        {
            return ((y - outerBox_.p1.y) * outerBox_.getWidth() +
                    (x - outerBox_.p1.x));
        }

        void copyElements(Grid &target) const
        {
            for (int y = innerBox_.p1.y; y < innerBox_.p2.y; ++y) {
                for (int x = innerBox_.p1.x; x < innerBox_.p2.x; ++x) {
                    target.elements_[target.getIndex(x, y)] = elements_[getIndex(x, y)];
                }
            }
        }
    };
}

#endif
