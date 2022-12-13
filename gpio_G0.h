#pragma once

#include "gpio.h"
#include "stm32g0xx.h"

template <size_t GPIO_ADDR> class GpioG0_LL;

template <> class GpioG0_LL<GPIOA_BASE>
{
  public:
    static void clock_enable()
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();
    }
};

template <> class GpioG0_LL<GPIOB_BASE>
{
  public:
    static void clock_enable()
    {
        __HAL_RCC_GPIOB_CLK_ENABLE();
    }
};

template <> class GpioG0_LL<GPIOC_BASE>
{
  public:
    static void clock_enable()
    {
        __HAL_RCC_GPIOC_CLK_ENABLE();
    }
};

template <> class GpioG0_LL<GPIOD_BASE>
{
  public:
    static void clock_enable()
    {
        __HAL_RCC_GPIOD_CLK_ENABLE();
    }
};

template <> class GpioG0_LL<GPIOF_BASE>
{
  public:
    static void clock_enable()
    {
        __HAL_RCC_GPIOF_CLK_ENABLE();
    }
};

template <size_t GPIO_ADDR> class GpioG0 : public GpioPortBase<GpioG0<GPIO_ADDR>>
{
    friend GpioPortBase<GpioG0<GPIO_ADDR>>;

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
        GpioG0_LL<GPIO_ADDR>::clock_enable();
        uint32_t pupd_bits = 0;
        uint32_t mode_bits = 0;
        uint32_t otype_bits = 0;
        uint32_t speed_bits = 0;

        switch (speed)
        {
        case GpioSpeed::LOW:
            speed_bits = 0b01;
            break;
        case GpioSpeed::MID:
            speed_bits = 0b10;
            break;
        case GpioSpeed::HIGH:
            speed_bits = 0b11;
            break;
        }

        switch (mode)
        {
        case GpioMode::INPUT:
            break;
        case GpioMode::ANALOG:
            mode_bits = 3;
            break;
        case GpioMode::INPUT_PULL_UP:
            pupd_bits = 1;
            break;
        case GpioMode::INPUT_PULL_DOWN:
            pupd_bits = 2;
            break;
        case GpioMode::OUTPUT:
            mode_bits = 1;
            break;
        case GpioMode::OPEN_DRAIN:
            otype_bits = 1;
            mode_bits = 1;
            break;
        }

        // const auto config_bits = (cnf_bits << 2) | mode_bits;
        // const auto crl = BitClone<uint32_t, 8, 4>::result(config_bits, mask & 0xFF);
        // const auto crh = BitClone<uint32_t, 8, 4>::result(config_bits, mask >> 8);
        // const auto crl_mask = BitClone<uint32_t, 8, 4>::result(0xF, mask & 0xFF);
        // const auto crh_mask = BitClone<uint32_t, 8, 4>::result(0xF, mask >> 8);
        // const auto old_odr = reinterpret_cast<GPIO_TypeDef *>(GPIO_ADDR)->ODR;
        // const auto old_crl = reinterpret_cast<GPIO_TypeDef *>(GPIO_ADDR)->CRL;
        // const auto old_crh = reinterpret_cast<GPIO_TypeDef *>(GPIO_ADDR)->CRH;
        // reinterpret_cast<GPIO_TypeDef *>(GPIO_ADDR)->ODR = (old_odr & (~mask)) | (output_init_state & mask);
        // reinterpret_cast<GPIO_TypeDef *>(GPIO_ADDR)->CRL = (old_crl & (~crl_mask)) | (crl & crl_mask);
        // reinterpret_cast<GPIO_TypeDef *>(GPIO_ADDR)->CRH = (old_crh & (~crh_mask)) | (crh & crh_mask);

        const auto moder = BitClone<uint32_t, 16, 2>::result(mode_bits, mask);
        const auto moder_mask = BitClone<uint32_t, 16, 2>::result(0x3, mask);
        const auto old_moder = reinterpret_cast<GPIO_TypeDef *>(GPIO_ADDR)->MODER;

        const auto otyper = BitClone<uint32_t, 16, 1>::result(otype_bits, mask);

        reinterpret_cast<GPIO_TypeDef *>(GPIO_ADDR)->MODER = (old_moder & (~moder_mask)) | (moder & moder_mask);
        reinterpret_cast<GPIO_TypeDef *>(GPIO_ADDR)->OTYPER = (otyper & (~mask)) | (otyper & mask);
    }
};