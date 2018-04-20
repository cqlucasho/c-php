<?php
/**
 * 服务类型条件查询
 */
function selectServerTypes($view) {
    $category = isset($_GET['category']) ? $_GET['category'] : '';
    $html = '<div class="btn-group">';
    $html .= '<button type="button" class="btn btn-default">'.serviceType(isset($_GET['server_type']) ? $_GET['server_type'] : 0).'</button>';
    $html .= '<button type="button" class="btn btn-default dropdown-toggle" data-toggle="dropdown" aria-haspopup="true" aria-expanded="false">';
    $html .= '<span class="caret"></span>';
    $html .= '<span class="sr-only">Toggle Dropdown</span></button>';
    $html .= '<ul class="dropdown-menu">';
    $html .= '<li><a href="'.$view->url('', '', '', array('server_type' => LotIncomeExpenses::SERVER_TYPE_LOTTERY, 'category' => $category)).'">兑奖促销</a></li>';
    $html .= '<li><a href="'.$view->url('', '', '', array('server_type' => LotIncomeExpenses::SERVER_TYPE_OTHER, 'category' => $category)).'">其他</a></li>';
    $html .= '<li class="divider"></li>';
    $html .= '<li><a href="'.$view->url('', '', '', array('server_type' => '', 'category' => $category)).'">取消</a></li>';
    $html .= '</ul></div>';

    echo $html;
}

/**
 * 费用分类条件查询
 */
function selectCategory($view) {
    $type = isset($_GET['server_type']) ? $_GET['server_type'] : '';
    $html = '<div class="btn-group">';
    $html .= '<button type="button" class="btn btn-default">'.category(isset($_GET['category']) ? $_GET['category'] : 0).'</button>';
    $html .= '<button type="button" class="btn btn-default dropdown-toggle" data-toggle="dropdown" aria-haspopup="true" aria-expanded="false">';
    $html .= '<span class="caret"></span>';
    $html .= '<span class="sr-only">Toggle Dropdown</span></button>';
    $html .= '<ul class="dropdown-menu">';
    $html .= '<li><a href="'.$view->url('', '', '', array('category' => LotIncomeExpenses::CATEGORY_CONTRACT, 'server_type' => $type)).'">合同费</a></li>';
    $html .= '<li><a href="'.$view->url('', '', '', array('category' => LotIncomeExpenses::CATEGORY_TAG, 'server_type' => $type)).'">标签费</a></li>';
    $html .= '<li><a href="'.$view->url('', '', '', array('category' => LotIncomeExpenses::CATEGORY_SMS, 'server_type' => $type)).'">短信费</a></li>';
    $html .= '<li><a href="'.$view->url('', '', '', array('category' => LotIncomeExpenses::CATEGORY_USE_CODE, 'server_type' => $type)).'">合同费(用码费)</a></li>';
    $html .= '<li><a href="'.$view->url('', '', '', array('category' => LotIncomeExpenses::CATEGORY_PRESENT_PREPARE, 'server_type' => $type)).'">礼品预付费</a></li>';
    $html .= '<li><a href="'.$view->url('', '', '', array('category' => LotIncomeExpenses::CATEGORY_PRESENT_BACK, 'server_type' => $type)).'">礼品退款费</a></li>';
    $html .= '<li><a href="'.$view->url('', '', '', array('category' => LotIncomeExpenses::CATEGORY_OTHER, 'server_type' => $type)).'">其它</a></li>';
    $html .= '<li><a href="'.$view->url('', '', '', array('category' => LotIncomeExpenses::CATEGORY_JD, 'server_type' => $type)).'">京东</a></li>';
    $html .= '<li><a href="'.$view->url('', '', '', array('category' => LotIncomeExpenses::CATEGORY_HONGBAO, 'server_type' => $type)).'">红包</a></li>';
    $html .= '<li><a href="'.$view->url('', '', '', array('category' => LotIncomeExpenses::CATEGORY_MOBILE, 'server_type' => $type)).'">话费</a></li>';
    $html .= '<li><a href="'.$view->url('', '', '', array('category' => LotIncomeExpenses::CATEGORY_ENTITY, 'server_type' => $type)).'">实物</a></li>';
    $html .= '<li><a href="'.$view->url('', '', '', array('category' => LotIncomeExpenses::CATEGORY_DISCOUNT, 'server_type' => $type)).'">优惠券</a></li>';
    $html .= '<li class="divider"></li>';
    $html .= '<li><a href="'.$view->url('', '', '', array('category' => '', 'server_type' => $type)).'">取消</a></li>';
    $html .= '</ul></div>';

    echo $html;
}

/**
 * 服务类型
 *
 * @param int $type
 * @return string
 */
function serviceType($type) {
    switch ($type) {
        case LotLotcodeServer::TYPE_LOTTERY:
            return '兑奖促销';
        case LotLotcodeServer::TYPE_OTHER:
            return '其它';
        default:
            return '请选择服务类型';
    }
}

/**
 * 类型
 *
 * @param int $type
 * @return string
 */
function feeType($type) {
    switch ($type) {
        case LotIncomeExpenses::FEE_TYPE_TOPUP:
            return '充值(到账)';
        case LotIncomeExpenses::FEE_TYPE_BACK:
            return '退款';
        case LotIncomeExpenses::FEE_TYPE_OTHER:
            return '消费';
        default:
            break;
    }
}

/**
 * 费用分类
 *
 * @param int $category
 * @return string
 */
function category($category) {
    switch ($category) {
        case LotIncomeExpenses::CATEGORY_CONTRACT:
            return '合同费';
        case LotIncomeExpenses::CATEGORY_TAG:
            return '标签费';
        case LotIncomeExpenses::CATEGORY_SMS:
            return '短信费';
        case LotIncomeExpenses::CATEGORY_USE_CODE:
            return '合同费(用码费)';
        case LotIncomeExpenses::CATEGORY_PRESENT_PREPARE:
            return '礼品预付费';
        case LotIncomeExpenses::CATEGORY_PRESENT_BACK:
            return '礼品退款费';
        case LotIncomeExpenses::CATEGORY_OTHER:
            return '其它';
        case LotIncomeExpenses::CATEGORY_JD:
            return '京东';
        case LotIncomeExpenses::CATEGORY_HONGBAO:
            return '红包';
        case LotIncomeExpenses::CATEGORY_MOBILE:
            return '话费';
        case LotIncomeExpenses::CATEGORY_ENTITY:
            return '实物';
        case LotIncomeExpenses::CATEGORY_DISCOUNT:
            return '优惠券';
        default:
            return '请选择费用类型';
            break;
    }
}