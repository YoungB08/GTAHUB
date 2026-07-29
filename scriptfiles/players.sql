<<<<<<< Updated upstream
CREATE TABLE IF NOT EXISTS players
(
    u_id int(11) NOT NULL AUTO_INCREMENT,
    account_id int(11) DEFAULT NULL,
    is_character tinyint(1) NOT NULL DEFAULT '0',
    username varchar(24) NOT NULL,
    password char(60) NOT NULL,
    firstname varchar(24) NOT NULL DEFAULT '',
    lastname varchar(24) NOT NULL DEFAULT '',
    age tinyint(3) NOT NULL DEFAULT '18',
    gender tinyint(1) NOT NULL DEFAULT '0',
    origin varchar(32) NOT NULL DEFAULT '',
    background varchar(96) NOT NULL DEFAULT '',
    register_date DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP(),
    last_login DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP(),
    PRIMARY KEY
        (u_id),
    UNIQUE KEY
        username (username)
);
=======
﻿-- SQL Schema file for players.sql
>>>>>>> Stashed changes
