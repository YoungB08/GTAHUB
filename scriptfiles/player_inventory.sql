<<<<<<< Updated upstream
CREATE TABLE IF NOT EXISTS player_inventory
(
    inv_id      int(11)         NOT NULL AUTO_INCREMENT,
    u_id        int(11)         NOT NULL,
    item_name   varchar(64)     NOT NULL COMMENT 'Ten vat pham (slug)',
    item_label  varchar(64)     NOT NULL DEFAULT '' COMMENT 'Ten hien thi',
    quantity    int(11)         NOT NULL DEFAULT '1',
    weight      float           NOT NULL DEFAULT '0.0' COMMENT 'Trong luong moi don vi (kg)',
    is_illegal  tinyint(1)      NOT NULL DEFAULT '0' COMMENT '0=hop le, 1=bat hop le',
    extra_data  varchar(255)    DEFAULT NULL COMMENT 'JSON cho vat pham dac biet (VD serial so pha co)',
    PRIMARY KEY (inv_id),
    FOREIGN KEY (u_id) REFERENCES players(u_id) ON DELETE CASCADE,
    INDEX idx_uid (u_id)
);

CREATE TABLE IF NOT EXISTS vehicle_trunk
(
    trunk_id    int(11)         NOT NULL AUTO_INCREMENT,
    vehicle_id  int(11)         NOT NULL COMMENT 'player_vehicles.vehicle_id',
    item_name   varchar(64)     NOT NULL,
    item_label  varchar(64)     NOT NULL DEFAULT '',
    quantity    int(11)         NOT NULL DEFAULT '1',
    weight      float           NOT NULL DEFAULT '0.0',
    PRIMARY KEY (trunk_id),
    INDEX idx_vehicleid (vehicle_id)
);
=======
﻿-- SQL Schema file for player_inventory.sql
>>>>>>> Stashed changes
