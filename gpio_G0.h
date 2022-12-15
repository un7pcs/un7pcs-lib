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
            pupd_bits = 0b01;
            break;
        case GpioMode::INPUT_PULL_DOWN:
            pupd_bits = 0b10;
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
        const auto speedr = BitClone<uint32_t, 16, 2>::result(speed_bits, mask);
        //const auto moder_temp = BitClone<uint32_t, 16, 2>::result(0b11, mask);
        const auto moder_mask = BitClone<uint32_t, 16, 2>::result(0x3, mask);
        const auto pupd = BitClone<uint32_t, 16, 2>::result(pupd_bits, mask);
        const auto otyper = BitClone<uint32_t, 16, 1>::result(otype_bits, mask);

        const auto old_moder = reinterpret_cast<GPIO_TypeDef *>(GPIO_ADDR)->MODER;
        const auto old_speedr = reinterpret_cast<GPIO_TypeDef *>(GPIO_ADDR)->OSPEEDR;
        const auto old_otyper = reinterpret_cast<GPIO_TypeDef *>(GPIO_ADDR)->OTYPER;
        const auto old_pupd = reinterpret_cast<GPIO_TypeDef *>(GPIO_ADDR)->PUPDR;
        const auto old_odr = reinterpret_cast<GPIO_TypeDef *>(GPIO_ADDR)->ODR;

        //reinterpret_cast<GPIO_TypeDef *>(GPIO_ADDR)->MODER = old_moder | moder_temp;
       
        reinterpret_cast<GPIO_TypeDef *>(GPIO_ADDR)->ODR = (old_odr & (~mask)) | (output_init_state & mask);
        reinterpret_cast<GPIO_TypeDef *>(GPIO_ADDR)->MODER = (old_moder & (~moder_mask)) | (moder & moder_mask);
        reinterpret_cast<GPIO_TypeDef *>(GPIO_ADDR)->OTYPER = (old_otyper & (~mask)) | (otyper & mask);
        reinterpret_cast<GPIO_TypeDef *>(GPIO_ADDR)->OSPEEDR = (old_speedr & (~mask)) | (speedr & mask);
        reinterpret_cast<GPIO_TypeDef *>(GPIO_ADDR)->PUPDR = (old_pupd & (~mask)) | (pupd & mask);
    }
};

typedef GpioG0<GPIOA_BASE> GpioA;
typedef GpioPin<GpioA, 0> PA0;
typedef GpioPin<GpioA, 1> PA1;
typedef GpioPin<GpioA, 2> PA2;
typedef GpioPin<GpioA, 3> PA3;
typedef GpioPin<GpioA, 4> PA4;
typedef GpioPin<GpioA, 5> PA5;
typedef GpioPin<GpioA, 6> PA6;
typedef GpioPin<GpioA, 7> PA7;
typedef GpioPin<GpioA, 8> PA8;
typedef GpioPin<GpioA, 9> PA9;
typedef GpioPin<GpioA, 10> PA10;
typedef GpioPin<GpioA, 11> PA11;
typedef GpioPin<GpioA, 12> PA12;
typedef GpioPin<GpioA, 13> PA13;
typedef GpioPin<GpioA, 14> PA14;
typedef GpioPin<GpioA, 15> PA15;

typedef GpioG0<GPIOB_BASE> GpioB;
typedef GpioPin<GpioB, 0> PB0;
typedef GpioPin<GpioB, 1> PB1;
typedef GpioPin<GpioB, 2> PB2;
typedef GpioPin<GpioB, 3> PB3;
typedef GpioPin<GpioB, 4> PB4;
typedef GpioPin<GpioB, 5> PB5;
typedef GpioPin<GpioB, 6> PB6;
typedef GpioPin<GpioB, 7> PB7;
typedef GpioPin<GpioB, 8> PB8;
typedef GpioPin<GpioB, 9> PB9;
typedef GpioPin<GpioB, 10> PB10;
typedef GpioPin<GpioB, 11> PB11;
typedef GpioPin<GpioB, 12> PB12;
typedef GpioPin<GpioB, 13> PB13;
typedef GpioPin<GpioB, 14> PB14;
typedef GpioPin<GpioB, 15> PB15;

typedef GpioG0<GPIOC_BASE> GpioC;
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

typedef GpioG0<GPIOD_BASE> GpioD;
typedef GpioPin<GpioD, 0> PD0;
typedef GpioPin<GpioD, 1> PD1;
typedef GpioPin<GpioD, 2> PD2;
typedef GpioPin<GpioD, 3> PD3;
typedef GpioPin<GpioD, 4> PD4;
typedef GpioPin<GpioD, 5> PD5;
typedef GpioPin<GpioD, 6> PD6;
typedef GpioPin<GpioD, 7> PD7;
typedef GpioPin<GpioD, 8> PD8;
typedef GpioPin<GpioD, 9> PD9;
typedef GpioPin<GpioD, 10> PD10;
typedef GpioPin<GpioD, 11> PD11;
typedef GpioPin<GpioD, 12> PD12;
typedef GpioPin<GpioD, 13> PD13;
typedef GpioPin<GpioD, 14> PD14;
typedef GpioPin<GpioD, 15> PD15;

typedef GpioG0<GPIOF_BASE> GpioF;
typedef GpioPin<GpioF, 0> PF0;
typedef GpioPin<GpioF, 1> PF1;
typedef GpioPin<GpioF, 2> PF2;
typedef GpioPin<GpioF, 3> PF3;
typedef GpioPin<GpioF, 4> PF4;
typedef GpioPin<GpioF, 5> PF5;
typedef GpioPin<GpioF, 6> PF6;
typedef GpioPin<GpioF, 7> PF7;
typedef GpioPin<GpioF, 8> PF8;
typedef GpioPin<GpioF, 9> PF9;
typedef GpioPin<GpioF, 10> PF10;
typedef GpioPin<GpioF, 11> PF11;
typedef GpioPin<GpioF, 12> PF12;
typedef GpioPin<GpioF, 13> PF13;
typedef GpioPin<GpioF, 14> PF14;
typedef GpioPin<GpioF, 15> PF15;