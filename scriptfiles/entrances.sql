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
﻿-- SQL Schema file for entrances.sql
>>>>>>> Stashed changes
