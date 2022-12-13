#pragma once

#include "gpio.h"

template <size_t GPIO_ADDR> class GpioF1_LL;

template <> class GpioF1_LL<GPIOC_BASE>
{
  public:
    static void clock_enable()
    {
        __HAL_RCC_GPIOC_CLK_ENABLE();
    }
};

template <> class GpioF1_LL<GPIOD_BASE>
{
  public:
    static void clock_enable()
    {
        __HAL_RCC_GPIOD_CLK_ENABLE();
    }
};

template <> class GpioF1_LL<GPIOE_BASE>
{
  public:
    static void clock_enable()
    {
        __HAL_RCC_GPIOE_CLK_ENABLE();
    }
};


template <size_t GPIO_ADDR> class GpioF1 : public GpioPortBase<GpioF1<GPIO_ADDR>>
{
    friend GpioPortBase<GpioF1<GPIO_ADDR>>;

  private:
    static void write_impl(const uint32_t data)
    {
        reinterpret_cast<GPIO_TypeDef *>(GPIO_ADDR)->ODR = data;
    }

    static void write_impl(const uint32_t data, const size_t mask)
    {
        const auto odr_value = reinterpret_cast<GPIO_TypeDef *>(GPIO_ADDR)->ODR;
        reinterpret_cast<GPIO_TypeDef *>(GPIO_ADDR)->ODR = (odr_value & (~mask)) | (data & mask);
    }

    static void set_impl(const size_t mask)
    {
        reinterpret_cast<GPIO_TypeDef *>(GPIO_ADDR)->BSRR = mask;
    }

    static void reset_impl(const size_t mask)
    {
        reinterpret_cast<GPIO_TypeDef *>(GPIO_ADDR)->BRR = mask;
    }

    static uint32_t read_impl()
    {
        return reinterpret_cast<GPIO_TypeDef *>(GPIO_ADDR)->IDR;
    }
    static void set_mode_impl(const GpioMode mode, const GpioSpeed speed, uint32_t output_init_state,
                              const uint32_t mask)
    {
        GpioF1_LL<GPIO_ADDR>::clock_enable();
        uint32_t cnf_bits = 0;
        uint32_t mode_bits = 0;
        switch (speed)
        {

        case GpioSpeed::LOW:
            mode_bits = 0b10;
            break;
        case GpioSpeed::MID:
            mode_bits = 0b01;
            break;
        case GpioSpeed::HIGH:
            mode_bits = 0b11;
            break;
        }
        switch (mode)
        {
        case GpioMode::INPUT:
            cnf_bits = 1;
            mode_bits = 0;
            break;
        case GpioMode::ANALOG:
            cnf_bits = 0;
            mode_bits = 0;
            break;
        case GpioMode::INPUT_PULL_UP:
            cnf_bits = 2;
            mode_bits = 0;
            output_init_state |= mask;
            break;
        case GpioMode::INPUT_PULL_DOWN:
            cnf_bits = 2;
            mode_bits = 0;
            output_init_state &= ~mask;
            break;
        case GpioMode::OUTPUT:
            cnf_bits = 0;
            break;
        case GpioMode::OPEN_DRAIN:
            cnf_bits = 1;
            break;
        }
        const auto config_bits = (cnf_bits << 2) | mode_bits;
        const auto crl = BitClone<uint32_t, 8, 4>::result(config_bits, mask & 0xFF);
        const auto crh = BitClone<uint32_t, 8, 4>::result(config_bits, mask >> 8);
        const auto crl_mask = BitClone<uint32_t, 8, 4>::result(0xF, mask & 0xFF);
        const auto crh_mask = BitClone<uint32_t, 8, 4>::result(0xF, mask >> 8);
        const auto old_odr = reinterpret_cast<GPIO_TypeDef *>(GPIO_ADDR)->ODR;
        const auto old_crl = reinterpret_cast<GPIO_TypeDef *>(GPIO_ADDR)->CRL;
        const auto old_crh = reinterpret_cast<GPIO_TypeDef *>(GPIO_ADDR)->CRH;
        reinterpret_cast<GPIO_TypeDef *>(GPIO_ADDR)->ODR = (old_odr & (~mask)) | (output_init_state & mask);
        reinterpret_cast<GPIO_TypeDef *>(GPIO_ADDR)->CRL = (old_crl & (~crl_mask)) | (crl & crl_mask);
        reinterpret_cast<GPIO_TypeDef *>(GPIO_ADDR)->CRH = (old_crh & (~crh_mask)) | (crh & crh_mask);
    }
};

// DEF

typedef GpioF1<GPIOA_BASE> GpioA;
typedef GpioPin<GpioA, 2> PA2;
typedef GpioPin<GpioA, 3> PA3;
typedef GpioPin<GpioA, 10> PA10;
typedef GpioPin<GpioA, 11> PA11;

typedef GpioF1<GPIOC_BASE> GpioC;
typedef GpioPin<GpioC, 0> PC0;
typedef GpioPin<GpioC, 1> PC1;
typedef GpioPin<GpioC, 2> PC2;
typedef GpioPin<GpioC, 3> PC3;
typedef GpioPin<GpioC, 4> PC4;
typedef GpioPin<GpioC, 5> PC5;
typedef GpioPin<GpioC, 6> PC6;
typedef GpioPin<GpioC, 7> PC7;
typedef GpioPin<GpioC, 8> PC8;
typedef GpioPin<GpioC, 9> PC9;
typedef GpioPin<GpioC, 10> PC10;
typedef GpioPin<GpioC, 11> PC11;
typedef GpioPin<GpioC, 12> PC12;
typedef GpioPin<GpioC, 13> PC13;
typedef GpioPin<GpioC, 14> PC14;
typedef GpioPin<GpioC, 15> PC15;

typedef GpioF1<GPIOD_BASE> GpioD;
typedef GpioPin<GpioD, 13> PD13;
typedef GpioPin<GpioD, 6> PD6;

typedef GpioF1<GPIOE_BASE> GpioE;
typedef GpioPin<GpioE, 5> PE5;
typedef GpioPin<GpioE, 4> PE4;
typedef GpioPin<GpioE, 3> PE3;
typedef GpioPin<GpioE, 2> PE2;

