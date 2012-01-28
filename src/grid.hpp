#ifndef CRUST_GRID_HPP
#define CRUST_GRID_HPP

#include "grid_box.hpp"

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
            return innerBox_.getX();
        }

        int getY() const
        {
            return innerBox_.getY();
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
            if (innerBox_.contains(x, y)) {
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
                GridBox box;
                for (int y = innerBox_.getY(); y < innerBox_.getY() + innerBox_.getHeight(); ++y) {
                    for (int x = innerBox_.getX(); x < innerBox_.getX() + innerBox_.getWidth(); ++x) {
                        if (elements_[getIndex(x, y)] != defaultValue_) {
                            box.add(x, y);
                        }
                    }
                }
                innerBox_ = box;
                normalized_ = true;
            }
        }

    private:
        explicit Grid(GridBox const &box, Element const &defaultValue) :
            innerBox_(box),
            outerBox_(box),
            defaultValue_(defaultValue),
            elements_(0),
            normalized_(true)
        {
            int dx = std::max(1, int(0.5f * M_SQRT2 * float(box.getWidth()) + 0.5f));
            int dy = std::max(1, int(0.5f * M_SQRT2 * float(box.getHeight()) + 0.5f));
            outerBox_.pad(dx, dy);
            elements_ = new Element[outerBox_.getSize()];
            std::fill(elements_, elements_ + outerBox_.getSize(), defaultValue);
        }

        // Noncopyable.
        //
        // TODO: Public implementations.
        Grid(Grid const &other);
        Grid &operator=(Grid const &other);

        GridBox innerBox_;
        GridBox outerBox_;
        Element defaultValue_;
        Element *elements_;
        bool normalized_;

        void addElement(int x, int y, Element const &value)
        {
            if (innerBox_.contains(x, y)) {
                elements_[getIndex(x, y)] = value;
            } else if (outerBox_.contains(x, y)) {
                innerBox_.add(x, y);
                elements_[getIndex(x, y)] = value;
            } else {
                normalize();
                
                GridBox box(innerBox_);
                box.add(x, y);
                
                Grid other(box, defaultValue_);
                copyElements(other);
                swap(other);
                
                elements_[getIndex(x, y)] = value;
            }
        }

        void removeElement(int x, int y)
        {
            if (innerBox_.contains(x, y)) {
                elements_[getIndex(x, y)] = defaultValue_;
                normalized_ = false;
            }
        }

        int getIndex(int x, int y) const
        {
            return ((y - outerBox_.getY()) * outerBox_.getWidth() +
                    (x - outerBox_.getX()));
        }

        void copyElements(Grid &target) const
        {
            int x = innerBox_.getX();
            int y = innerBox_.getY();
            int width = innerBox_.getWidth();
            int height = innerBox_.getHeight();
            for (int dy = 0; dy < height; ++dy) {
                for (int dx = 0; dx < width; ++dx) {
                    int index = getIndex(x + dx, y + dy);
                    int targetIndex = target.getIndex(x + dx, y + dy);
                    target.elements_[targetIndex] = elements_[index];
                }
            }
        }
    };
}

#endif
