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
