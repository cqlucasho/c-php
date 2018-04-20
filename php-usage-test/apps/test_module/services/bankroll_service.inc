<?php
Ant\Core::import('bankroll/models/lot_income_expenses', 'path_apps');
Ant\Core::import('bankroll/models/lot_expenses_stat_day', 'path_apps');
/**
 * 资金账户服务类
 *
 * @author lucasho
 * @created 2017-05-24 15:49
 * @modified 2017-05-24 15:49
 * @version 1.0
 */
class BankrollService extends Ant\AService {
    /**
     * 根据$condition和$params获取所有充值记录
     *
     * @param string $condition
     * @param array $params
     * @return array
     */
    public static function getTopupList($condition, array $params) {
        $sql = sprintf(self::$GET_ALL_TOPUP, $params['fields'], LotIncomeExpenses::$name, $condition, $params['order'], ' limit '.$params['limit']);

        return self::db()->query($sql)->getRows();
    }

    /**
     * 获取所有充值记录
     * @var string $GET_ALL_TOPUP
     */
    protected static $GET_ALL_TOPUP = 'select %s from %s where %s order by %s %s';
}
