-- Combined GTAHUB SQL Database Schema

-- ==============================================
-- FILE: admins.sql
-- ==============================================
<<<<<<< Updated upstream
CREATE TABLE IF NOT EXISTS admins (
    u_id INT(11) NOT NULL,
    admin_level TINYINT(2) NOT NULL DEFAULT 0,
    PRIMARY KEY
        (u_id),
    FOREIGN KEY
        (u_id)
    REFERENCES
        players(u_id)
    ON DELETE
        CASCADE
    ON UPDATE
        NO ACTION
);
=======
ï»¿-- SQL Schema file for admins.sql
>>>>>>> Stashed changes


-- ==============================================
-- FILE: armys.sql
-- ==============================================
<<<<<<< Updated upstream
CREATE TABLE IF NOT EXISTS armys (
    u_id int(11) NOT NULL,
    PRIMARY KEY
        (u_id),
    FOREIGN KEY
        (u_id)
    REFERENCES
        players (u_id)
    ON DELETE
        CASCADE
    ON UPDATE
        NO ACTION
);
=======
ï»¿-- SQL Schema file for armys.sql
>>>>>>> Stashed changes


-- ==============================================
-- FILE: atms.sql
-- ==============================================
CREATE TABLE IF NOT EXISTS atms (
    atm_id int(11) NOT NULL AUTO_INCREMENT,
    atm_x float NOT NULL,
    atm_y float NOT NULL,
    atm_z float NOT NULL,
    rx float NOT NULL DEFAULT 0,
    ry float NOT NULL DEFAULT 0,
    rz float NOT NULL DEFAULT 0,
    wid tinyint(6) NOT NULL,
    interior tinyint(6) NOT NULL,
    PRIMARY KEY (atm_id)
);

INSERT IGNORE INTO atms (atm_x, atm_y, atm_z, rx, ry, rz, wid, interior) VALUES
(2234.733398, 51.345561000, 26.134365, 0.0, 0.0, 0.000000, 0, 0),
(1381.069213, 259.56204200, 19.156929, 0.0, 0.0, 157.0000, 0, 0),
(661.3598020, -555.1714470, 15.965932, 0.0, 0.0, -90.0000, 0, 0),
(-2177.50292, -2435.006591, 30.214990, 0.0, 0.0, 52.00000, 0, 0),
(1367.251464, -1284.611938, 13.156874, 0.0, 0.0, -90.6000, 0, 0),
(1928.592651, -1771.088012, 13.172806, 0.0, 0.0, 90.00000, 0, 0),
(2323.767333, -1644.993896, 14.442724, 0.0, 0.0, 0.000000, 0, 0),
(2043.748779, -1416.704711, 16.810766, 0.0, 0.0, -90.0000, 0, 0),
(2387.751464, -1981.961669, 13.156866, 0.0, 0.0, -180.000, 0, 0),
(1494.450195, -1768.979492, 18.365745, 0.0, 0.0, -90.0000, 0, 0),
(1051.627075, -1026.406616, 31.661567, 0.0, 0.0, 0.000000, 0, 0),
(816.8725580, -1356.521240, 13.156099, 0.0, 0.0, -180.000, 0, 0),
(1808.732177, -1567.267822, 13.063967, 0.0, 0.0, 37.00000, 0, 0),
(2412.541259, -1492.666992, 23.628126, 0.0, 0.0, -180.000, 0, 0),
(2431.131347, -1219.477539, 25.022165, 0.0, 0.0, 0.000000, 0, 0),
(255.4551690, -197.5846250, 1.2381240, 0.0, 0.0, -90.0000, 0, 0);


-- ==============================================
-- FILE: bans.sql
-- ==============================================
<<<<<<< Updated upstream
CREATE TABLE IF NOT EXISTS bans (
    username VARCHAR(24),
    ip VARCHAR(17),
    gcpi VARCHAR(60),
    reason VARCHAR(32),
    admin VARCHAR(24),
    ban_date datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
    unban_date datetime,
    UNIQUE KEY username (username)
);
=======
ï»¿-- SQL Schema file for bans.sql
>>>>>>> Stashed changes


-- ==============================================
-- FILE: businesses.sql
-- ==============================================
-- scriptfiles/businesses.sql
-- Báº£ng lÆ°u Doanh Nghiá»‡p Do NgÆ°á»i ChÆ¡i Sá»Ÿ Há»¯u (business_economy.inc)
-- LÆ°u Ã½: báº£ng nÃ y Ä‘Æ°á»£c táº¡o/nÃ¢ng cáº¥p tá»± Ä‘á»™ng lÃºc server khá»Ÿi Ä‘á»™ng (OnMySQLConnected
-- trong core/systems/business_economy.inc). File nÃ y chá»‰ dÃ¹ng Ä‘á»ƒ tham kháº£o cáº¥u trÃºc.

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
    -- GiÃ¡ bÃ¡n riÃªng cho tá»«ng máº·t hÃ ng cá»§a cá»­a hÃ ng 24/7 (chá»§ tiá»‡m tá»± Ä‘áº·t qua /bizpanel)
    -- 0 = Burger, 1 = Nuoc uong, 2 = Bo So Cuu, 3 = Can Cau Ca
    `item_price_0` INT(11)     NOT NULL DEFAULT 15,
    `item_price_1` INT(11)     NOT NULL DEFAULT 10,
    `item_price_2` INT(11)     NOT NULL DEFAULT 150,
    `item_price_3` INT(11)     NOT NULL DEFAULT 50,
    PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;


-- ==============================================
-- FILE: characters.sql
-- ==============================================
<<<<<<< Updated upstream
CREATE TABLE IF NOT EXISTS characters
(
    char_id         int(11)         NOT NULL AUTO_INCREMENT,
    u_id            int(11)         NOT NULL COMMENT 'Foreign key -> players.u_id',

    -- ThÃ´ng tin nhÃ¢n váº­t IC
    firstname       varchar(32)     NOT NULL DEFAULT '' COMMENT 'Ten IC (Firstname)',
    lastname        varchar(32)     NOT NULL DEFAULT '' COMMENT 'Ho IC (Lastname)',
    age             int(3)          NOT NULL DEFAULT '20',
    gender          tinyint(1)      NOT NULL DEFAULT '0' COMMENT '0=Nam, 1=Nu',

    -- Giao diá»‡n
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

    -- Vá»‹ trÃ­ thoÃ¡t láº§n cuá»‘i
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
ï»¿-- SQL Schema file for characters.sql
>>>>>>> Stashed changes


-- ==============================================
-- FILE: entrances.sql
-- ==============================================
<<<<<<< Updated upstream
CREATE TABLE IF NOT EXISTS entrances (
    entrance_id int(11) NOT NULL AUTO_INCREMENT,
    name varchar(30) NOT NULL DEFAULT 'LOI VAO',
    map_icon smallint(4) NOT NULL DEFAULT '0',
    freeze_player tinyint(1) NOT NULL DEFAULT '0',
    entry_x float NOT NULL,
    entry_y float NOT NULL,
    entry_z float NOT NULL,
    entry_a float NOT NULL DEFAULT '0.0',
    entry_world int(11) NOT NULL DEFAULT '0',
    entry_interior int(11) NOT NULL DEFAULT '0',
    exit_x float NOT NULL,
    exit_y float NOT NULL,
    exit_z float NOT NULL,
    exit_a float NOT NULL DEFAULT '0.0',
    exit_world int(11) NOT NULL DEFAULT '0',
    exit_interior int(11) NOT NULL DEFAULT '0',
    enabled tinyint(1) NOT NULL DEFAULT '1',
    created_at datetime NOT NULL DEFAULT CURRENT_TIMESTAMP(),
    updated_at datetime NOT NULL DEFAULT CURRENT_TIMESTAMP() ON UPDATE CURRENT_TIMESTAMP(),
    PRIMARY KEY (entrance_id)
);
=======
ï»¿-- SQL Schema file for entrances.sql
>>>>>>> Stashed changes


-- ==============================================
-- FILE: gangs.sql
-- ==============================================
CREATE TABLE IF NOT EXISTS gangs (
    id INT(11) NOT NULL AUTO_INCREMENT,
    leader_uid INT(11) NOT NULL,
    name VARCHAR(30) NOT NULL,
    colour BIGINT(11) NOT NULL DEFAULT -1,
    kills INT(11) NOT NULL DEFAULT 0,
    deaths INT(11) NOT NULL DEFAULT 0,
    PRIMARY KEY
        (id),
    FOREIGN KEY
        (leader_uid)
    REFERENCES
        players(u_id)
    ON DELETE
        CASCADE
    ON UPDATE
        NO ACTION
);
CREATE TABLE IF NOT EXISTS gang_members (
    gang_id INT(11) NOT NULL AUTO_INCREMENT,
    user_id INT(11) NOT NULL,
    gang_rank INT(11) NOT NULL DEFAULT 1,
    PRIMARY KEY
        (user_id),
    FOREIGN KEY
        (user_id) REFERENCES players(u_id),
    FOREIGN KEY
        (gang_id) REFERENCES gangs(id)
    ON DELETE
        CASCADE
    ON UPDATE
        NO ACTION
);

-- ==============================================
-- FILE: houses.sql
-- ==============================================
<<<<<<< Updated upstream
    CREATE TABLE IF NOT EXISTS player_houses (
        house_id SMALLINT NOT NULL DEFAULT 0,
        u_id INT(11) NULL DEFAULT NULL,
        description VARCHAR(32) DEFAULT 'No description',
        price MEDIUMINT NOT NULL,
        money_stored MEDIUMINT NOT NULL,
        interior TINYINT NOT NULL DEFAULT 0,
        entrance_x FLOAT NOT NULL,
        entrance_y FLOAT NOT NULL,
        entrance_z FLOAT NOT NULL,
        entrance_a FLOAT NOT NULL,
        interior_id TINYINT(4) NOT NULL,
        virtual_world MEDIUMINT NOT NULL,
        UNIQUE KEY
            (house_id),
        FOREIGN KEY
            (u_id)
        REFERENCES
            players(u_id)
        ON DELETE
            SET NULL
        ON UPDATE
            NO ACTION
    );

    CREATE TABLE IF NOT EXISTS house_settings (
        house_id smallint(6) NOT NULL,
        locked tinyint(1) NOT NULL DEFAULT 0,
        PRIMARY KEY
            (house_id),
        FOREIGN KEY
            (house_id)
        REFERENCES
            player_houses (house_id)
        ON DELETE
            CASCADE
        ON UPDATE
            CASCADE
    );

    CREATE TABLE IF NOT EXISTS house_key_holders (
        houseid smallint(6) NOT NULL,
        uid int(11) NOT NULL,
        PRIMARY KEY (houseid),
        KEY uid (uid),
        FOREIGN KEY
            (houseid)
        REFERENCES
            player_houses (house_id)
        ON DELETE
            CASCADE
        ON UPDATE
            CASCADE,
        FOREIGN KEY
            (uid)
        REFERENCES
            players (u_id)
        ON DELETE
            CASCADE
        ON UPDATE
            CASCADE
    );

    CREATE TABLE IF NOT EXISTS house_furniture (
        furniture_id INT(11) AUTO_INCREMENT,
        house_id SMALLINT(6) NOT NULL,
        object_id INT(11) NOT NULL,
        x FLOAT(6) NOT NULL,
        y FLOAT(6) NOT NULL,
        z FLOAT(6) NOT NULL,
        rx FLOAT(6) NOT NULL,
        ry FLOAT(6) NOT NULL,
        rz FLOAT(6) NOT NULL,
        intid INT(11) NOT NULL,
        vworld INT(11) NOT NULL,
        INDEX
            (house_id),
        PRIMARY KEY
            (furniture_id),
        FOREIGN KEY
            (house_id)
        REFERENCES
            player_houses(house_id)
        ON DELETE
            CASCADE
        ON UPDATE
            NO ACTION
    );
=======
ï»¿-- SQL Schema file for houses.sql
>>>>>>> Stashed changes


-- ==============================================
-- FILE: jail.sql
-- ==============================================
<<<<<<< Updated upstream
CREATE TABLE IF NOT EXISTS jailed (
    u_id int(11) NOT NULL AUTO_INCREMENT,
    bail_required tinyint(4) NOT NULL DEFAULT '0',
    jailed_time mediumint(8) NOT NULL DEFAULT '0',
    bail_balance mediumint(8) NOT NULL DEFAULT '0',
    PRIMARY KEY
        (u_id),
    FOREIGN KEY
        (u_id) REFERENCES players(u_id)
    ON DELETE
        CASCADE
    ON UPDATE
        NO ACTION
);
=======
ï»¿-- SQL Schema file for jail.sql
>>>>>>> Stashed changes


-- ==============================================
-- FILE: job_skills.sql
-- ==============================================
-- scriptfiles/job_skills.sql
-- Báº£ng lÆ°u Level/XP Nghá» nghiá»‡p theo tá»«ng Job cho má»—i nhÃ¢n váº­t

CREATE TABLE IF NOT EXISTS `job_skills` (
    `id`          INT(11)      NOT NULL AUTO_INCREMENT,
    `char_id`     INT(11)      NOT NULL DEFAULT 0,
    `job_id`      TINYINT(4)   NOT NULL DEFAULT 0,
    -- job_id: 1=Trucker, 2=Pizza, 3=Garbage, 4=RoadCleaner, 5=Mechanic
    `xp`          INT(11)      NOT NULL DEFAULT 0,
    `level`       TINYINT(4)   NOT NULL DEFAULT 1,
    PRIMARY KEY (`id`),
    UNIQUE KEY `char_job` (`char_id`, `job_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;


-- ==============================================
-- FILE: phone.sql
-- ==============================================
<<<<<<< Updated upstream
CREATE TABLE IF NOT EXISTS phone_numbers
(
    u_id        int(11)     NOT NULL,
    phone_num   varchar(10) NOT NULL UNIQUE COMMENT 'So dien thoai 7 chu so (dau 09x...)',
    PRIMARY KEY (u_id),
    FOREIGN KEY (u_id)
    REFERENCES players(u_id)
    ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS phone_contacts
(
    id          int(11)     NOT NULL AUTO_INCREMENT,
    owner_uid   int(11)     NOT NULL,
    name        varchar(32) NOT NULL COMMENT 'Ten luu trong danh ba',
    number      varchar(10) NOT NULL,
    PRIMARY KEY (id),
    FOREIGN KEY (owner_uid) REFERENCES players(u_id) ON DELETE CASCADE
);
=======
ï»¿-- SQL Schema file for phone.sql
>>>>>>> Stashed changes


-- ==============================================
-- FILE: players.sql
-- ==============================================
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
ï»¿-- SQL Schema file for players.sql
>>>>>>> Stashed changes


-- ==============================================
-- FILE: players_characters.sql
-- ==============================================
<<<<<<< Updated upstream
SET @gtahub_schema = DATABASE();

SET @gtahub_sql = IF(
    (SELECT COUNT(*) FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_SCHEMA = @gtahub_schema AND TABLE_NAME = 'players' AND COLUMN_NAME = 'account_id') = 0,
    'ALTER TABLE players ADD COLUMN account_id int(11) DEFAULT NULL AFTER u_id',
    'DO 1'
);
PREPARE gtahub_stmt FROM @gtahub_sql;
EXECUTE gtahub_stmt;
DEALLOCATE PREPARE gtahub_stmt;

SET @gtahub_sql = IF(
    (SELECT COUNT(*) FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_SCHEMA = @gtahub_schema AND TABLE_NAME = 'players' AND COLUMN_NAME = 'is_character') = 0,
    'ALTER TABLE players ADD COLUMN is_character tinyint(1) NOT NULL DEFAULT ''0'' AFTER account_id',
    'DO 1'
);
PREPARE gtahub_stmt FROM @gtahub_sql;
EXECUTE gtahub_stmt;
DEALLOCATE PREPARE gtahub_stmt;

SET @gtahub_sql = IF(
    (SELECT COUNT(*) FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_SCHEMA = @gtahub_schema AND TABLE_NAME = 'players' AND COLUMN_NAME = 'firstname') = 0,
    'ALTER TABLE players ADD COLUMN firstname varchar(24) NOT NULL DEFAULT '''' AFTER password',
    'DO 1'
);
PREPARE gtahub_stmt FROM @gtahub_sql;
EXECUTE gtahub_stmt;
DEALLOCATE PREPARE gtahub_stmt;

SET @gtahub_sql = IF(
    (SELECT COUNT(*) FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_SCHEMA = @gtahub_schema AND TABLE_NAME = 'players' AND COLUMN_NAME = 'lastname') = 0,
    'ALTER TABLE players ADD COLUMN lastname varchar(24) NOT NULL DEFAULT '''' AFTER firstname',
    'DO 1'
);
PREPARE gtahub_stmt FROM @gtahub_sql;
EXECUTE gtahub_stmt;
DEALLOCATE PREPARE gtahub_stmt;

SET @gtahub_sql = IF(
    (SELECT COUNT(*) FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_SCHEMA = @gtahub_schema AND TABLE_NAME = 'players' AND COLUMN_NAME = 'age') = 0,
    'ALTER TABLE players ADD COLUMN age tinyint(3) NOT NULL DEFAULT ''18'' AFTER lastname',
    'DO 1'
);
PREPARE gtahub_stmt FROM @gtahub_sql;
EXECUTE gtahub_stmt;
DEALLOCATE PREPARE gtahub_stmt;

SET @gtahub_sql = IF(
    (SELECT COUNT(*) FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_SCHEMA = @gtahub_schema AND TABLE_NAME = 'players' AND COLUMN_NAME = 'gender') = 0,
    'ALTER TABLE players ADD COLUMN gender tinyint(1) NOT NULL DEFAULT ''0'' AFTER age',
    'DO 1'
);
PREPARE gtahub_stmt FROM @gtahub_sql;
EXECUTE gtahub_stmt;
DEALLOCATE PREPARE gtahub_stmt;

SET @gtahub_sql = IF(
    (SELECT COUNT(*) FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_SCHEMA = @gtahub_schema AND TABLE_NAME = 'players' AND COLUMN_NAME = 'origin') = 0,
    'ALTER TABLE players ADD COLUMN origin varchar(32) NOT NULL DEFAULT '''' AFTER gender',
    'DO 1'
);
PREPARE gtahub_stmt FROM @gtahub_sql;
EXECUTE gtahub_stmt;
DEALLOCATE PREPARE gtahub_stmt;

SET @gtahub_sql = IF(
    (SELECT COUNT(*) FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_SCHEMA = @gtahub_schema AND TABLE_NAME = 'players' AND COLUMN_NAME = 'background') = 0,
    'ALTER TABLE players ADD COLUMN background varchar(96) NOT NULL DEFAULT '''' AFTER origin',
    'DO 1'
);
PREPARE gtahub_stmt FROM @gtahub_sql;
EXECUTE gtahub_stmt;
DEALLOCATE PREPARE gtahub_stmt;
=======
ï»¿-- SQL Schema file for players_characters.sql
>>>>>>> Stashed changes


-- ==============================================
-- FILE: player_bank.sql
-- ==============================================
<<<<<<< Updated upstream
CREATE TABLE IF NOT EXISTS player_bank (
    u_id int(11) NOT NULL,
    money int(11) NOT NULL DEFAULT 0,
    PRIMARY KEY
        (u_id),
    FOREIGN KEY
        (u_id)
    REFERENCES
    players (u_id)
    ON DELETE
        CASCADE
    ON UPDATE
        NO ACTION
);
=======
ï»¿-- SQL Schema file for player_bank.sql
>>>>>>> Stashed changes


-- ==============================================
-- FILE: player_hits.sql
-- ==============================================
<<<<<<< Updated upstream
CREATE TABLE IF NOT EXISTS player_hits (
    u_id int(11) NOT NULL,
    amount int(11) NOT NULL DEFAULT '0',
    reason varchar(32) NOT NULL DEFAULT '',
    placed_by varchar(24) NOT NULL DEFAULT '',
    created_at DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP(),
    PRIMARY KEY
        (u_id),
    FOREIGN KEY
        (u_id)
    REFERENCES
        players(u_id)
    ON DELETE
        CASCADE
    ON UPDATE
        NO ACTION
);
=======
ï»¿-- SQL Schema file for player_hits.sql
>>>>>>> Stashed changes


-- ==============================================
-- FILE: player_inventory.sql
-- ==============================================
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
ï»¿-- SQL Schema file for player_inventory.sql
>>>>>>> Stashed changes


-- ==============================================
-- FILE: player_items.sql
-- ==============================================
<<<<<<< Updated upstream
CREATE TABLE IF NOT EXISTS player_items (
    u_id int(11) NOT NULL,
    c4 tinyint(4) NOT NULL DEFAULT 0,
    crack tinyint(4) NOT NULL DEFAULT 0,
    weed tinyint(4) NOT NULL DEFAULT 0,
    picklock tinyint(4) NOT NULL DEFAULT 0,
    wallet tinyint(4) NOT NULL DEFAULT 0,
    rope tinyint(4) NOT NULL DEFAULT 0,
    condom tinyint(4) NOT NULL DEFAULT 0,
    scissors tinyint(4) NOT NULL DEFAULT 0,
    oranges tinyint(4) NOT NULL DEFAULT 0,
    bottles tinyint(4) NOT NULL DEFAULT 0,
    orange_juice tinyint(4) NOT NULL DEFAULT 0,
    fruit_picker tinyint(4) NOT NULL DEFAULT 0,
    UNIQUE KEY
        u_id (u_id),
    FOREIGN KEY
        (u_id)
    REFERENCES
        players (u_id)
    ON DELETE
        CASCADE
    ON UPDATE
        NO ACTION
);
=======
ï»¿-- SQL Schema file for player_items.sql
>>>>>>> Stashed changes


-- ==============================================
-- FILE: player_job_progress.sql
-- ==============================================
<<<<<<< Updated upstream
CREATE TABLE IF NOT EXISTS player_job_progress (
    u_id INT NOT NULL,
    job_id SMALLINT NOT NULL,
    job_xp INT NOT NULL DEFAULT 0,
    job_level SMALLINT NOT NULL DEFAULT 1,
    PRIMARY KEY (u_id, job_id),
    FOREIGN KEY (u_id) REFERENCES players(u_id) ON DELETE CASCADE ON UPDATE CASCADE
);
=======
ï»¿-- SQL Schema file for player_job_progress.sql
>>>>>>> Stashed changes


-- ==============================================
-- FILE: player_licenses.sql
-- ==============================================
<<<<<<< Updated upstream
CREATE TABLE IF NOT EXISTS player_licenses (
    u_id INT NOT NULL,
    license_id TINYINT NOT NULL,
    issued_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (u_id, license_id),
    FOREIGN KEY (u_id) REFERENCES players(u_id) ON DELETE CASCADE ON UPDATE CASCADE
);
=======
ï»¿-- SQL Schema file for player_licenses.sql
>>>>>>> Stashed changes


-- ==============================================
-- FILE: player_phones.sql
-- ==============================================
<<<<<<< Updated upstream
CREATE TABLE IF NOT EXISTS player_phones (
    u_id INT NOT NULL,
    phone_number INT NOT NULL UNIQUE,
    created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (u_id),
    FOREIGN KEY (u_id) REFERENCES players(u_id) ON DELETE CASCADE ON UPDATE CASCADE
);

CREATE TABLE IF NOT EXISTS player_phone_contacts (
    u_id INT NOT NULL,
    contact_name VARCHAR(24) NOT NULL,
    phone_number INT NOT NULL,
    PRIMARY KEY (u_id, phone_number),
    FOREIGN KEY (u_id) REFERENCES players(u_id) ON DELETE CASCADE ON UPDATE CASCADE
);
=======
ï»¿-- SQL Schema file for player_phones.sql
>>>>>>> Stashed changes


-- ==============================================
-- FILE: player_positions.sql
-- ==============================================
<<<<<<< Updated upstream
CREATE TABLE IF NOT EXISTS player_positions (
    u_id int(11) NOT NULL,
    x float NOT NULL DEFAULT '0.0',
    y float NOT NULL DEFAULT '0.0',
    z float NOT NULL DEFAULT '0.0',
    angle float NOT NULL DEFAULT '0.0',
    world int(11) NOT NULL DEFAULT '0',
    interior int(11) NOT NULL DEFAULT '0',
    updated_at datetime NOT NULL DEFAULT CURRENT_TIMESTAMP() ON UPDATE CURRENT_TIMESTAMP(),
    PRIMARY KEY (u_id),
    FOREIGN KEY (u_id)
        REFERENCES players(u_id)
        ON DELETE CASCADE
        ON UPDATE NO ACTION
);
=======
ï»¿-- SQL Schema file for player_positions.sql
>>>>>>> Stashed changes


-- ==============================================
-- FILE: player_shots.sql
-- ==============================================
<<<<<<< Updated upstream

CREATE TABLE IF NOT EXISTS player_shots_stats (
    uid int(11) NOT NULL,
    chest int(11) NOT NULL DEFAULT 0,
    torso int(11) NOT NULL DEFAULT 0,
    left_arm int(11) NOT NULL DEFAULT 0,
    right_arm int(11) NOT NULL DEFAULT 0,
    left_leg int(11) NOT NULL DEFAULT 0,
    right_leg int(11) NOT NULL DEFAULT 0,
    head int(11) NOT NULL DEFAULT 0,
    misses int(11) NOT NULL DEFAULT 0,
    PRIMARY KEY
        (uid),
    FOREIGN KEY
        (uid)
    REFERENCES
        players (u_id)
    ON DELETE
        CASCADE
    ON UPDATE
        NO ACTION
);
=======
ï»¿-- SQL Schema file for player_shots.sql
>>>>>>> Stashed changes


-- ==============================================
-- FILE: player_stats.sql
-- ==============================================
<<<<<<< Updated upstream
CREATE TABLE IF NOT EXISTS player_stats
(
    u_id int(11) NOT NULL AUTO_INCREMENT,
    armour float NOT NULL DEFAULT '100.0',
    health float NOT NULL DEFAULT '100.0',
    kills mediumint(8) NOT NULL DEFAULT '0',
    deaths mediumint(8) NOT NULL DEFAULT '0',
    job_id tinyint(4) NOT NULL DEFAULT '0',
    class_id tinyint(4) NOT NULL DEFAULT '0',
    wanted_level mediumint(6) NOT NULL DEFAULT '0',
    player_bounty mediumint(8) NOT NULL DEFAULT '0',
    money bigint(20) NOT NULL DEFAULT '0',
    score int(11) NOT NULL DEFAULT '0',
    skin smallint(4) NOT NULL DEFAULT '0',
    xp int(11) NOT NULL DEFAULT '0',
    level smallint(4) NOT NULL DEFAULT '1',
    PRIMARY KEY
        (u_id),
    FOREIGN KEY
        (u_id)
    REFERENCES
        players(u_id)
    ON DELETE
        CASCADE
    ON UPDATE
        NO ACTION
);
=======
ï»¿-- SQL Schema file for player_stats.sql
>>>>>>> Stashed changes


-- ==============================================
-- FILE: player_vehicles_plate.sql
-- ==============================================
-- SQL Schema file for player_vehicles_plate.sql


-- ==============================================
-- FILE: turfs.sql
-- ==============================================
<<<<<<< Updated upstream
CREATE TABLE IF NOT EXISTS turfs
(
    turf_id     int(11)         NOT NULL AUTO_INCREMENT,
    name        varchar(64)     NOT NULL,
    x           float           NOT NULL,
    y           float           NOT NULL,
    z           float           NOT NULL,
    radius      float           NOT NULL DEFAULT '80.0',
    gang_id     int(11)                  DEFAULT NULL COMMENT 'NULL = khong co chu',
    color       int(11)         NOT NULL DEFAULT '0x808080FF' COMMENT 'Mau hien thi tren map',
    last_capture datetime                DEFAULT NULL,
    PRIMARY KEY (turf_id)
);

-- Dá»¯ liá»‡u máº«u: CÃ¡c turf cá»§a LS
INSERT IGNORE INTO turfs (turf_id, name, x, y, z, radius, gang_id) VALUES
(1,  'Ganton',           2435.0, -1698.0, 13.5, 100.0, NULL),
(2,  'Glen Park',        1795.0, -1584.0, 13.4, 90.0,  NULL),
(3,  'Idlewood',         1870.0, -1869.0, 13.2, 100.0, NULL),
(4,  'East Beach',       2593.0, -1508.0, 14.0, 90.0,  NULL),
(5,  'Jefferson',        2096.0, -1770.0, 13.3, 80.0,  NULL),
(6,  'Willowfield',      2376.0, -1928.0, 13.1, 80.0,  NULL),
(7,  'Playa Del Seville', 2636.0,-1848.0, 13.6, 80.0,  NULL),
(8,  'El Corona',        1970.0, -2112.0, 13.5, 90.0,  NULL),
(9,  'Little Mexico',    1703.0, -2082.0, 13.5, 80.0,  NULL),
(10, 'Chamberlain Hills', 2118.0,-1835.0, 13.4, 85.0,  NULL);
=======
ï»¿-- SQL Schema file for turfs.sql
>>>>>>> Stashed changes


-- ==============================================
-- FILE: vehicle_fuel.sql
-- ==============================================
<<<<<<< Updated upstream
CREATE TABLE IF NOT EXISTS vehicle_fuel
(
    vehicle_id  int(11)         NOT NULL,
    fuel        float           NOT NULL DEFAULT '100.0' COMMENT 'Lua hien tai (0.0 - 100.0)',
    PRIMARY KEY (vehicle_id),
    FOREIGN KEY (vehicle_id)
    REFERENCES player_vehicles(vehicle_id)
    ON DELETE CASCADE
    ON UPDATE NO ACTION
);
=======
ï»¿-- SQL Schema file for vehicle_fuel.sql
>>>>>>> Stashed changes


-- ==============================================
-- FILE: vips.sql
-- ==============================================
<<<<<<< Updated upstream
CREATE TABLE IF NOT EXISTS vips (
    u_id INT(11) AUTO_INCREMENT,
    vip_level TINYINT(2) NOT NULL DEFAULT 0,
    vip_expire_date datetime NULL,
    PRIMARY KEY
        (u_id),
    FOREIGN KEY
        (u_id)
    REFERENCES
        players(u_id)
    ON DELETE
        CASCADE
    ON UPDATE
        NO ACTION
);
=======
ï»¿-- SQL Schema file for vips.sql
>>>>>>> Stashed changes


-- ==============================================
-- FILE: weapons.sql
-- ==============================================
<<<<<<< Updated upstream
CREATE TABLE IF NOT EXISTS player_weapons
(
    u_id int(11) NOT NULL AUTO_INCREMENT,
    weapid int(11) NOT NULL,
    ammo int(30) NOT NULL,
    UNIQUE KEY
        (u_id, weapid),
    FOREIGN KEY
        (u_id)
    REFERENCES
        players(u_id)
    ON DELETE
        CASCADE
    ON UPDATE
        NO ACTION
);
=======
ï»¿-- SQL Schema file for weapons.sql
>>>>>>> Stashed changes



