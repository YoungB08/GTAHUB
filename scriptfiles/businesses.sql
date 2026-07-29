-- scriptfiles/businesses.sql
-- Bảng lưu Doanh Nghiệp Do Người Chơi Sở Hữu (business_economy.inc)
-- Lưu ý: bảng này được tạo/nâng cấp tự động lúc server khởi động (OnMySQLConnected
-- trong core/systems/business_economy.inc). File này chỉ dùng để tham khảo cấu trúc.

CREATE TABLE IF NOT EXISTS `businesses` (
    `id`          INT(11)      NOT NULL AUTO_INCREMENT,
    `owner_uid`   INT(11)      NOT NULL DEFAULT 0,
    `name`        VARCHAR(64)  NOT NULL DEFAULT 'Doanh nghiep vo chu',
    `type`        INT(11)      NOT NULL DEFAULT 1,
    -- type: 1 = 24/7, 2 = Ammunation
    `price`       INT(11)      NOT NULL DEFAULT 50000,
    `safe_money`  INT(11)      NOT NULL DEFAULT 0,
    `stock_level` INT(11)      NOT NULL DEFAULT 1000,
    `pos_x`       FLOAT        NOT NULL DEFAULT 0,
    `pos_y`       FLOAT        NOT NULL DEFAULT 0,
    `pos_z`       FLOAT        NOT NULL DEFAULT 0,
    `vw`          INT(11)      NOT NULL DEFAULT 0,
    `interior`    INT(11)      NOT NULL DEFAULT 0,
    -- Giá bán riêng cho từng mặt hàng của cửa hàng 24/7 (chủ tiệm tự đặt qua /bizpanel)
    -- 0 = Burger, 1 = Nuoc uong, 2 = Bo So Cuu, 3 = Can Cau Ca
    `item_price_0` INT(11)     NOT NULL DEFAULT 15,
    `item_price_1` INT(11)     NOT NULL DEFAULT 10,
    `item_price_2` INT(11)     NOT NULL DEFAULT 150,
    `item_price_3` INT(11)     NOT NULL DEFAULT 50,
    PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
