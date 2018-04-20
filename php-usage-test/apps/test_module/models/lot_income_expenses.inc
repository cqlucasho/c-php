<?php
class LotIncomeExpenses {
    /**
     * 数据库表名
     * @var string $name
     */
    public static $name = 'lot_income_expenses';
    /**
     * 数据库表别名
     * @var string $alias
     */
    public static $alias = 'lot_income_expenses as LotIncomeExpenses';

    /**
     * 1:兑奖促销 2:其它
     */
    const SERVER_TYPE_LOTTERY = 1;
    const SERVER_TYPE_OTHER = 2;

    /**
     * 1:充值(到账) 2:退款 3:消费
     */
    const FEE_TYPE_TOPUP = 1;
    const FEE_TYPE_BACK = 2;
    const FEE_TYPE_OTHER = 3;

    /**
     * 1:合同费|2:标签费|3:短信费|4:合同费(用码费)|5:礼品预付费|6:礼品退款费|7:其它|8:京东|9:红包|10:话费|11:实物|12:优惠券
     */
    const CATEGORY_CONTRACT         = 1;
    const CATEGORY_TAG              = 2;
    const CATEGORY_SMS              = 3;
    const CATEGORY_USE_CODE         = 4;
    const CATEGORY_PRESENT_PREPARE  = 5;
    const CATEGORY_PRESENT_BACK     = 6;
    const CATEGORY_OTHER            = 7;
    const CATEGORY_JD               = 8;
    const CATEGORY_HONGBAO          = 9;
    const CATEGORY_MOBILE           = 10;
    const CATEGORY_ENTITY           = 11;
    const CATEGORY_DISCOUNT         = 12;
}