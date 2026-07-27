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
