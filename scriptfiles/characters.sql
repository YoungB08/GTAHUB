<<<<<<< Updated upstream
CREATE TABLE IF NOT EXISTS characters
(
    char_id         int(11)         NOT NULL AUTO_INCREMENT,
    u_id            int(11)         NOT NULL COMMENT 'Foreign key -> players.u_id',

    -- Thông tin nhân vật IC
    firstname       varchar(32)     NOT NULL DEFAULT '' COMMENT 'Ten IC (Firstname)',
    lastname        varchar(32)     NOT NULL DEFAULT '' COMMENT 'Ho IC (Lastname)',
    age             int(3)          NOT NULL DEFAULT '20',
    gender          tinyint(1)      NOT NULL DEFAULT '0' COMMENT '0=Nam, 1=Nu',

    -- Giao diện
    skin_id         int(4)          NOT NULL DEFAULT '0',

    -- Stats IC
    money           int(11)         NOT NULL DEFAULT '5000' COMMENT 'Tien mat ban dau',
    bank_money      int(11)         NOT NULL DEFAULT '0',
    kills           int(11)         NOT NULL DEFAULT '0',
    deaths          int(11)         NOT NULL DEFAULT '0',
    job_id          int(4)          NOT NULL DEFAULT '0',
    class_id        tinyint(2)      NOT NULL DEFAULT '0',
    wanted_level    int(2)          NOT NULL DEFAULT '0',
    score           int(11)         NOT NULL DEFAULT '0',
    xp              int(11)         NOT NULL DEFAULT '0',

    -- Vị trí thoát lần cuối
    pos_x           float           NOT NULL DEFAULT '1543.4' COMMENT 'Spawn mac dinh: LS Hospital',
    pos_y           float           NOT NULL DEFAULT '-1675.6',
    pos_z           float           NOT NULL DEFAULT '13.5',
    pos_a           float           NOT NULL DEFAULT '180.0',

    -- Metadata
    created_at      datetime        DEFAULT CURRENT_TIMESTAMP,
    last_played     datetime        DEFAULT NULL,

    PRIMARY KEY (char_id),
    FOREIGN KEY (u_id) REFERENCES players(u_id) ON DELETE CASCADE,
    UNIQUE KEY unique_fullname (firstname, lastname)
);
=======
﻿-- SQL Schema file for characters.sql
>>>>>>> Stashed changes
