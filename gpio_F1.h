#pragma once

#include "gpio.h"
#include "stm32f1xx_hal.h"

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

typedef GpioF1<GPIOB_BASE> GpioB;
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
typedef GpioPin<GpioD, 0>   PD0;
typedef GpioPin<GpioD, 1>   PD1;
typedef GpioPin<GpioD, 2>   PD2;
typedef GpioPin<GpioD, 3>   PD3;
typedef GpioPin<GpioD, 4>   PD4;
typedef GpioPin<GpioD, 5>   PD5;
typedef GpioPin<GpioD, 6>   PD6;
typedef GpioPin<GpioD, 7>   PD7;
typedef GpioPin<GpioD, 8>   PD8;
typedef GpioPin<GpioD, 9>   PD9;
typedef GpioPin<GpioD, 10>  PD10;
typedef GpioPin<GpioD, 11>  PD11;
typedef GpioPin<GpioD, 12>  PD12;
typedef GpioPin<GpioD, 13>  PD13;
typedef GpioPin<GpioD, 14>  PD14;
typedef GpioPin<GpioD, 15>  PD15;


typedef GpioF1<GPIOE_BASE> GpioE;
typedef GpioPin<GpioE, 0>   PE0;
typedef GpioPin<GpioE, 1>   PE1;
typedef GpioPin<GpioE, 2>   PE2;
typedef GpioPin<GpioE, 3>   PE3;
typedef GpioPin<GpioE, 4>   PE4;
typedef GpioPin<GpioE, 5>   PE5;
typedef GpioPin<GpioE, 6>   PE6;
typedef GpioPin<GpioE, 7>   PE7;
typedef GpioPin<GpioE, 8>   PE8;
typedef GpioPin<GpioE, 9>   PE9;
typedef GpioPin<GpioE, 10>  PE10;
typedef GpioPin<GpioE, 11>  PE11;
typedef GpioPin<GpioE, 12>  PE12;
typedef GpioPin<GpioE, 13>  PE13;
typedef GpioPin<GpioE, 14>  PE14;
typedef GpioPin<GpioE, 15>  PE15;

typedef GpioF1<GPIOF_BASE> GpioF;
typedef GpioPin<GpioF, 0>   PF0;
typedef GpioPin<GpioF, 1>   PF1;
typedef GpioPin<GpioF, 2>   PF2;
typedef GpioPin<GpioF, 3>   PF3;
typedef GpioPin<GpioF, 4>   PF4;
typedef GpioPin<GpioF, 5>   PF5;
typedef GpioPin<GpioF, 6>   PF6;
typedef GpioPin<GpioF, 7>   PF7;
typedef GpioPin<GpioF, 8>   PF8;
typedef GpioPin<GpioF, 9>   PF9;
typedef GpioPin<GpioF, 10>  PF10;
typedef GpioPin<GpioF, 11>  PF11;
typedef GpioPin<GpioF, 12>  PF12;
typedef GpioPin<GpioF, 13>  PF13;
typedef GpioPin<GpioF, 14>  PF14;
typedef GpioPin<GpioF, 15>  PF15;


typedef GpioF1<GPIOG_BASE> GpioG;
typedef GpioPin<GpioG, 0>   PG0;
typedef GpioPin<GpioG, 1>   PG1;
typedef GpioPin<GpioG, 2>   PG2;
typedef GpioPin<GpioG, 3>   PG3;
typedef GpioPin<GpioG, 4>   PG4;
typedef GpioPin<GpioG, 5>   PG5;
typedef GpioPin<GpioG, 6>   PG6;
typedef GpioPin<GpioG, 7>   PG7;
typedef GpioPin<GpioG, 8>   PG8;
typedef GpioPin<GpioG, 9>   PG9;
typedef GpioPin<GpioG, 10>  PG10;
typedef GpioPin<GpioG, 11>  PG11;
typedef GpioPin<GpioG, 12>  PG12;
typedef GpioPin<GpioG, 13>  PG13;
typedef GpioPin<GpioG, 14>  PG14;
typedef GpioPin<GpioG, 15>  PG15;

