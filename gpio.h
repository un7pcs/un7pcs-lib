#pragma once

#include "typelist.h"
#include <stdint.h>

enum class GpioMode
{
    ANALOG,
    INPUT,
    INPUT_PULL_UP,
    INPUT_PULL_DOWN,
    OUTPUT,
    OPEN_DRAIN
};

enum class GpioSpeed
{
    LOW,
    MID,
    HIGH
};

// CRTP interface

template <class T> class GpioPortBase
{
  public:
    static void write(const uint32_t data)
    {
        T::write_impl(data);
    }
    static void write(const uint32_t data, const uint32_t mask)
    {
        T::write_impl(data, mask);
    }
    static void set(const uint32_t mask)
    {
        T::set_impl(mask);
    }
    static void reset(const uint32_t mask)
    {
        T::reset_impl(mask);
    }
    static uint32_t read()
    {
        return T::read_impl();
    }
    static void set_mode(const GpioMode mode, const GpioSpeed speed = GpioSpeed::HIGH,
                         const uint32_t init_output_state = 0, const uint32_t mask = 0xFFFFFFFF)
    {
        T::set_mode_impl(mode, speed, init_output_state, mask);
    }
};

template <typename BASE_TYPE, int N, int SIZE> struct BitClone;

template <typename BASE_TYPE, int N, int SIZE> struct BitClone
{
    static constexpr BASE_TYPE result(const BASE_TYPE value, const BASE_TYPE mask)
    {
        return ((mask & 1) ? value : 0) | (BitClone<BASE_TYPE, (N - 1), SIZE>::result(value, (mask >> 1)) << SIZE);
    }
};

template <typename BASE_TYPE, int SIZE> struct BitClone<BASE_TYPE, 0, SIZE>
{
    static constexpr BASE_TYPE result(const BASE_TYPE value, const BASE_TYPE mask)
    {
        return 0;
    }
};

template <class PORT, size_t BIT> class GpioPin
{
  public:
    typedef PORT PortClass;
    constexpr static size_t BIT_MASK = 1 << BIT;
    constexpr static size_t BIT_NUMBER = BIT;

    static void set()
    {
        PORT::set(BIT_MASK);
    }

    static void reset()
    {
        PORT::reset(BIT_MASK);
    }

    static void write(const bool bit_value)
    {
        if (bit_value == true)
            set();
        else
            reset();
    }

    static bool read()
    {
        return (PORT::read() & BIT_MASK) != 0;
    }

    static void set_mode(const GpioMode mode, const GpioSpeed speed = GpioSpeed::HIGH, bool init_output_state = false)
    {
        PORT::set_mode(mode, speed, init_output_state ? (1 << BIT_NUMBER) : 0, BIT_MASK);
    }
};

/******************************************************/
/*             Шаблон GetPorts                        */
/*             Формирует список портов, которые	      */
/*             используются в указанном списке выводов*/
/******************************************************/

// шаблон принимает список линий в качестве параметра
template <typename List> struct GetPorts;
// для пустого списка результат – пустой тип
template <> struct GetPorts<SimpleTL::NullType>
{
    typedef SimpleTL::NullType AllPorts;
};

template <typename T> struct GetPorts<SimpleTL::TypeList<T>>
{
    typedef SimpleTL::TypeList<typename T::PortClass> AllPorts;
};

// для непустого списка
// конкретизируем, что это должен быть список типов
// содержащий голову Head и хвост Tail
template <typename Head, typename... Tail> struct GetPorts<SimpleTL::TypeList<Head, Tail...>>
{
  private:
    // класс TPin помнит свой порт
    // запоминаем этот тип порта
    typedef typename Head::PortClass Port;
    // рекурсивно генерируем хвост
    typedef typename GetPorts<SimpleTL::TypeList<Tail...>>::AllPorts L1;

  public:
    // определяем список портов из текущего порта (Port) и хвоста (L1)
    typedef typename SimpleTL::Append<Port, L1>::Result AllPorts;
    // конвертируем список линий в соответствующий список портов
    // typedef typename GetPorts<TList>::Result PinsToPorts;
    // генерируем список портов без дудликатов
    typedef typename SimpleTL::NoDuplicates<AllPorts>::Result Ports;
};

/****************************************************
 *			Шаблон класса  GetPortMask				*
 *	Генерирует битовую маску для списка пинов       *
 * **************************************************/

template <class PinList> struct GetPortMask;

template <> struct GetPortMask<SimpleTL::NullType>
{
    constexpr static uint32_t value = 0;
};

template <class... T> struct GetPortMask<SimpleTL::TypeList<T...>>
{
    constexpr static uint32_t value =
        SimpleTL::TypeList<T...>::Head::BIT_MASK | GetPortMask<typename SimpleTL::TypeList<T...>::Tail>::value;
};

/****************************************************
 *     Шаблон      GetPinsWithPort     		        *
 *     Генерирует список пинов, принадлежащих ука-   *
 *     занному порту									*
 * **************************************************/
template <typename PinList, typename Port> struct GetPinsWithPort;

template <typename Port> struct GetPinsWithPort<SimpleTL::NullType, Port>
{
    typedef SimpleTL::NullType Result;
};

template <typename Port, typename... TAIL, size_t PIN>
struct GetPinsWithPort<SimpleTL::TypeList<GpioPin<Port, PIN>, TAIL...>, Port>
{
    typedef
        typename SimpleTL::Append<GpioPin<Port, PIN>,
                                  typename GetPinsWithPort<SimpleTL::TypeList<TAIL...>, Port>::Result>::Result Result;
};

template <typename... LIST, typename Port> struct GetPinsWithPort<SimpleTL::TypeList<LIST...>, Port>
{
    typedef typename GetPinsWithPort<typename SimpleTL::TypeList<LIST...>::Tail, Port>::Result Result;
};

/****************************************************************
 *     Шаблон      ShiftedValue       		                    *
 *     Сдвигает биты PinsToWrite виртуального порта  AllPins		*
 *     таким образом, чтобы они отображались                     *
 *     на биты физического порта, в котором они                  *
 *     находятся                                                 *
 * ***************************************************************/
template <typename PINS_TO_WRITE, typename ALL_PINS> struct ShiftedValue;

template <typename... PINS_TO_WRITE, typename... ALL_PINS>
struct ShiftedValue<SimpleTL::TypeList<PINS_TO_WRITE...>, SimpleTL::TypeList<ALL_PINS...>>
{
    typedef typename SimpleTL::TypeList<PINS_TO_WRITE...>::Head head;
    typedef typename SimpleTL::TypeList<PINS_TO_WRITE...>::Tail tail;
    typedef typename SimpleTL::TypeList<ALL_PINS...> AllPins;

  public:
    constexpr static uint32_t Result(const uint32_t value)
    {
        return ((1 << SimpleTL::IndexOf<AllPins, head>::Value) & value ? head::BIT_MASK : 0) |
               ShiftedValue<tail, AllPins>::Result(value);
    }
    constexpr static uint32_t ReverseResult(const uint32_t value)
    {
        return (head::BIT_MASK & value ? 1 << SimpleTL::IndexOf<AllPins, head>::Value : 0) |
               ShiftedValue<tail, AllPins>::ReverseResult(value);
    }
};

template <typename ALL_PINS> struct ShiftedValue<SimpleTL::NullType, ALL_PINS>
{
  public:
    constexpr static uint32_t Result(const uint32_t value)
    {
        return 0;
    }
    constexpr static uint32_t ReverseResult(const uint32_t value)
    {
        return 0;
    }
};

template <typename PORT_LIST, typename PIN_LIST> struct PortIterator;

template <typename... PORTS, typename... PIN_LIST>
struct PortIterator<SimpleTL::TypeList<PORTS...>, SimpleTL::TypeList<PIN_LIST...>>
{
    typedef typename SimpleTL::TypeList<PORTS...> PortList;
    typedef typename PortList::Head ThisPort;
    typedef typename SimpleTL::TypeList<PIN_LIST...> AllPins;
    typedef typename GetPinsWithPort<AllPins, typename PortList::Head>::Result ThisPortPins;

  public:
    static void write_mask(const uint32_t data, const uint32_t mask)
    {
        ThisPort::write(ShiftedValue<ThisPortPins, AllPins>::Result(data),
                        GetPortMask<ThisPortPins>::value & ShiftedValue<ThisPortPins, AllPins>::Result(mask));
        PortIterator<typename PortList::Tail, AllPins>::write_mask(data, mask);
    }
    static void set_mask(const uint32_t mask)
    {
        ThisPort::set(GetPortMask<ThisPortPins>::value & ShiftedValue<ThisPortPins, AllPins>::Result(mask));
        PortIterator<typename PortList::Tail, AllPins>::set_mask(mask);
    }
    static void reset_mask(const uint32_t mask)
    {
        ThisPort::reset(GetPortMask<ThisPortPins>::value & ShiftedValue<ThisPortPins, AllPins>::Result(mask));
        PortIterator<typename PortList::Tail, AllPins>::reset_mask(mask);
    }
    static uint32_t read()
    {
        return ShiftedValue<ThisPortPins, AllPins>::ReverseResult(ThisPort::read()) |
               PortIterator<typename PortList::Tail, AllPins>::read();
    }
    static void set_mode(const GpioMode mode, const GpioSpeed speed, const uint32_t init_output_state, const uint32_t mask)
    {
        ThisPort::set_mode(
            mode, speed, GetPortMask<ThisPortPins>::value & ShiftedValue<ThisPortPins, AllPins>::Result(init_output_state),
            GetPortMask<ThisPortPins>::value & ShiftedValue<ThisPortPins, AllPins>::Result(mask));
        PortIterator<typename PortList::Tail, AllPins>::set_mode(mode, speed, init_output_state, mask);
    }
};
template <typename... PIN_LIST> struct PortIterator<SimpleTL::NullType, SimpleTL::TypeList<PIN_LIST...>>
{
    static void write_mask(const uint32_t data, const uint32_t mask){};
    static void set_mask(const uint32_t mask){};
    static void reset_mask(const uint32_t mask){};
    static uint32_t read()
    {
        return 0;
    };
    static void set_mode(const GpioMode mode, const GpioSpeed speed, const uint32_t init_output_state, const uint32_t mask){};
};

// Virtual Port   VirtualPort<LSB, ... ,USB>
template <typename... PinList> class VirtualPort : public GpioPortBase<VirtualPort<PinList...>>
{
  public:
    typedef SimpleTL::TypeList<PinList...> Pins;

  public:
    // typedef typename GetPorts<PL>::AllPorts AP;
    typedef typename GetPorts<Pins>::Ports Ports;
    static void write_impl(const uint32_t data, const uint32_t mask)
    {
        PortIterator<Ports, Pins>::write_mask(data, mask);
    }
    static void write_impl(const uint32_t data)
    {
        PortIterator<Ports, Pins>::write_mask(data, 0xFFFFFFFF);
    }
    static void set_impl(const uint32_t mask)
    {
        PortIterator<Ports, Pins>::set_mask(mask);
    }
    static void set_impl()
    {
        PortIterator<Ports, Pins>::set_mask(0xFFFFFFFF);
    }
    static void reset_impl(const uint32_t mask)
    {
        PortIterator<Ports, Pins>::reset_mask(mask);
    }
    static void reset_impl()
    {
        PortIterator<Ports, Pins>::reset_mask(0xFFFFFFFF);
    }
    static uint32_t read_impl()
    {
        return PortIterator<Ports, Pins>::read();
    }
    static void set_mode_impl(const GpioMode mode)
    {
        PortIterator<Ports, Pins>::set_mode(mode, 0xFFFFFFFF);
    }
    static void set_mode_impl(const GpioMode mode, const GpioSpeed speed, const uint32_t init_output_state, const uint32_t mask)
    {
        PortIterator<Ports, Pins>::set_mode(mode, speed, init_output_state, mask);
    }
};
