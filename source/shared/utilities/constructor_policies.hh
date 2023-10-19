// This file is part of the IRCP Project. See LICENSE file for Copyright information

#pragma once

namespace IRCP
{
    /**
     * The non_copyable class is a policy class that provides interface for classes that should not be copyable.
     */
    class non_copyable
    {
    public:
        non_copyable() = default;
        ~non_copyable() = default;

        non_copyable(const non_copyable&) = delete;
        non_copyable& operator=(const non_copyable&) = delete;
    };

    /**
     * The non_movable class is a policy class that provides interface for classes that should not be movable.
     */
    class non_movable
    {
    public:
        non_movable() = default;
        ~non_movable() = default;

        non_movable(non_movable&&) = delete;
        non_movable& operator=(non_movable&&) = delete;
    };

    /**
     * The immovable class is a policy class that provides interface for classes that should be neither copyable nor
     * movable.
     */
    class immovable : private non_copyable, private non_movable { };
}
