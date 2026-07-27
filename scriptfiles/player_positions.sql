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
