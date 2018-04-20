<?php
Ant\Core::import('bankroll/services/bankroll_service', 'path_apps');
Ant\Core::import('myservice/models/lot_lotcode_server', 'path_apps');
/**
 * 资金统计控制器, 用于资金统计相关的业务逻辑处理.
 *
 * @author lucasho
 * @created 2017-05-26
 * @modified 2017-06-30
 * @version 1.0
 */
class IndexController extends Ant\AppController {
    /**
     * 资金账户首页
     */
    public function indexAction() {
        $userInfo = $this->getUserInfo();
        $params = array('order' => '_create_time desc', 'limit' => $this->paginate(20));
        $condition = "_company_id = {$userInfo['_company_id']}";

        $this->assign('account', BankrollService::getAccount($condition));
        $this->assign('statistics', BankrollService::getStatistics($condition, $params));
        $this->assign('statisticsCount', BankrollService::getStatisticsCount($condition));
        $this->render('index');
    }
}