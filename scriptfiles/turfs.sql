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

-- Dữ liệu mẫu: Các turf của LS
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
