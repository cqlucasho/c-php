<?php Ant\Core::import('myservice/views/__helpers/function', 'path_apps'); ?>
<!doctype html>
<html xmlns="http://www.w3.org/1999/html">
<head>
    <title>骆驼码 - 资金账户</title>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=Edge">
    <meta http-equiv="Content-Type" content="text/html">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <meta name="keywords" content="" />
    <meta name="description" content="" />
    <?php $this->inflateFile('header_style', PATH_APPS.'user/views/publics');?>
</head>
<body>
<div class="container-fluid">
    <?php $this->inflateFile('admin_header', PATH_APPS.'user/views/publics'); ?>
</div>

<?php $account = $this->viewVars('account'); ?>
<div class="container-fluid">
    <div class="row">
        <div class="col-md-2 admin-side">
            <?php $this->inflateFile('side', PATH_APPS.'user/views/publics'); ?>
        </div>
        <div class="col-md-10 admin-bankroll-index admin-content">
            <div class="row info marginT-40">
                <div class="col-md-3">
                    <ul class="clear-margin">
                        <li class="fs-16 c-black1">公司账户余额(元)：<?=formatAmount($account['_prepay']) - formatAmount($account['_cost']);?></li>
                        <li class="fs-16 c-black1">活动充值总额(元)：<?=formatAmount($account['_prepay']);?></li>
                        <li class="fs-16 c-black1">服务费用总额(元)：<?=formatAmount($account['_service_fee']);?></li>
                    </ul>
                </div>
                <div class="col-md-2 marginT-30 pull-right">
                    <a href="<?=$this->url('', 'flow', 'index');?>"
                        class="btn bg-orange1 fs-14 c-white">收支明细</a>
                </div>
            </div>

            <div class="row marginT-20">
                <div class="col-md-10">
                    <nav class="table-responsive">
                        <?php $statistics = $this->viewVars('statistics'); if(!empty($statistics)) {?>
                        <table class="table table-bordered">
                            <thead class="bg-gray">
                                <tr>
                                    <th class="text-center text-bold">服务名称</th>
                                    <th class="text-center text-bold">服务类型</th>
                                    <th class="text-center text-bold">费用金额</th>
                                    <th class="text-center text-bold">三天平均费用</th>
                                    <th class="text-center text-bold">更新时间</th>
                                </tr>
                            </thead>
                            <tbody class="bg-white">
                                <?php foreach($statistics as $statistic) {?>
                                    <tr class="text-center">
                                        <td><?=$statistic['serverName'];?></td>
                                        <td><?=type($statistic['_service_type']);?></td>
                                        <td><?=formatAmount($statistic['_spent']);?></td>
                                        <td><?=formatAmount($statistic['_three_day_avg_spent']);?></td>
                                        <td><?=dateTimeFormat($statistic['_create_time']);?></td>
                                    </tr>
                                <?php } ?>
                            </tbody>
                        </table>

                        <nav aria-label="Page navigation">
                            <?php echo $this->pages(20, isset($_GET['page'])?$_GET['page']:1, $this->viewVars('statisticsCount'));?>
                        </nav>
                        <?php }else{ ?>
                            <h4 class="c-gray">暂无记录信息</h4>
                        <?php } ?>
                    </nav>
                </div>
            </div>
        </div>
    </div>
</div>

<div class="container-fluid">
    <?php $this->inflateFile('admin_footer', PATH_APPS.'user/views/publics'); ?>
</div>
</body>
</html>