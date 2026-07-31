CREATE TABLE IF NOT EXISTS `custom_weapons` (
  `id` INT AUTO_INCREMENT PRIMARY KEY,
  `name` VARCHAR(64) NOT NULL,
  `base_weapon_id` INT NOT NULL,
  `damage_override` FLOAT NOT NULL DEFAULT 25.0,
  `tier` VARCHAR(32) NOT NULL DEFAULT 'Common',
  `max_ammo` INT NOT NULL DEFAULT 250,
  `price` INT NOT NULL DEFAULT 5000,
  `description` VARCHAR(128) NOT NULL DEFAULT '',
  `enabled` TINYINT(1) NOT NULL DEFAULT 1,
  `created_at` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE IF NOT EXISTS `player_custom_weapons` (
  `id` INT AUTO_INCREMENT PRIMARY KEY,
  `u_id` INT NOT NULL,
  `weapon_def_id` INT NOT NULL,
  `ammo` INT NOT NULL DEFAULT 100,
  `durability` FLOAT NOT NULL DEFAULT 100.0,
  `is_equipped` TINYINT(1) NOT NULL DEFAULT 0,
  `obtained_at` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
  FOREIGN KEY (`weapon_def_id`) REFERENCES `custom_weapons`(`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

INSERT INTO `custom_weapons` (`id`, `name`, `base_weapon_id`, `damage_override`, `tier`, `max_ammo`, `price`, `description`, `enabled`) VALUES
(1, 'Desert Eagle Chrome', 24, 55.0, 'Epic', 200, 15000, 'Sung ngan Deagle ma chrome sang bong voi sat thuong cao.', 1),
(2, 'AK-47 Dragon', 30, 42.0, 'Legendary', 500, 35000, 'Sung truong AK-47 kham hoa van Rong vang ruc ro.', 1),
(3, 'M4A1 VIP', 31, 38.0, 'Mythic', 600, 50000, 'Sung M4A1 VIP phien ban dac biet toc do ban va sat thuong cuc cao.', 1),
(4, 'Sniper Cobra', 34, 120.0, 'Legendary', 100, 60000, 'Sung ban tia Snipe Rắn Độc gay sat thuong chi mang.', 1),
(5, 'Shotgun Tactical', 25, 45.0, 'Rare', 150, 12000, 'Sung san Shotgun bien the quan dung sat thuong dien rong.', 1),
(6, 'MP5 SpecOps', 29, 28.0, 'Rare', 400, 10000, 'Sung tieu lien MP5 danh cho luc luong dac nhiem.', 1),
(7, 'Katana Shadow', 8, 75.0, 'Epic', 1, 8000, 'Kiem Katana Bong Toi chem xuyen giap.', 1),
(8, 'Combat Shotgun Enforcer', 27, 50.0, 'Mythic', 250, 45000, 'Shotgun tu dong Enforcer voi uy luc ap đảo.', 1)
ON DUPLICATE KEY UPDATE
  `name` = VALUES(`name`),
  `damage_override` = VALUES(`damage_override`),
  `tier` = VALUES(`tier`);
