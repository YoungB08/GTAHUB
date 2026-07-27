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
