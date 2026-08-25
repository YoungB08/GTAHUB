/*
 Navicat Premium Dump SQL

 Source Server         : localhost_3306
 Source Server Type    : MySQL
 Source Server Version : 80045 (8.0.45)
 Source Host           : localhost:3306
 Source Schema         : gtahub

 Target Server Type    : MySQL
 Target Server Version : 80045 (8.0.45)
 File Encoding         : 65001

 Date: 25/08/2026 21:53:45
*/

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for admins
-- ----------------------------
DROP TABLE IF EXISTS `admins`;
CREATE TABLE `admins`  (
  `u_id` int NOT NULL,
  `admin_level` tinyint NOT NULL DEFAULT 0,
  PRIMARY KEY (`u_id`) USING BTREE,
  CONSTRAINT `admins_ibfk_1` FOREIGN KEY (`u_id`) REFERENCES `players` (`u_id`) ON DELETE CASCADE ON UPDATE RESTRICT
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of admins
-- ----------------------------
INSERT INTO `admins` VALUES (1, 5);
INSERT INTO `admins` VALUES (4, 5);

-- ----------------------------
-- Table structure for armys
-- ----------------------------
DROP TABLE IF EXISTS `armys`;
CREATE TABLE `armys`  (
  `u_id` int NOT NULL,
  PRIMARY KEY (`u_id`) USING BTREE,
  CONSTRAINT `armys_ibfk_1` FOREIGN KEY (`u_id`) REFERENCES `players` (`u_id`) ON DELETE CASCADE ON UPDATE RESTRICT
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of armys
-- ----------------------------

-- ----------------------------
-- Table structure for atms
-- ----------------------------
DROP TABLE IF EXISTS `atms`;
CREATE TABLE `atms`  (
  `atm_id` int NOT NULL AUTO_INCREMENT,
  `atm_x` float NOT NULL,
  `atm_y` float NOT NULL,
  `atm_z` float NOT NULL,
  `rx` float NOT NULL DEFAULT 0,
  `ry` float NOT NULL DEFAULT 0,
  `rz` float NOT NULL DEFAULT 0,
  `wid` tinyint NOT NULL,
  `interior` tinyint NOT NULL,
  PRIMARY KEY (`atm_id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 17 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of atms
-- ----------------------------
INSERT INTO `atms` VALUES (1, 2234.73, 51.3456, 26.1344, 0, 0, 0, 0, 0);
INSERT INTO `atms` VALUES (2, 1381.07, 259.562, 19.1569, 0, 0, 157, 0, 0);
INSERT INTO `atms` VALUES (3, 661.36, -555.171, 15.9659, 0, 0, -90, 0, 0);
INSERT INTO `atms` VALUES (4, -2177.5, -2435.01, 30.215, 0, 0, 52, 0, 0);
INSERT INTO `atms` VALUES (5, 1367.25, -1284.61, 13.1569, 0, 0, -90.6, 0, 0);
INSERT INTO `atms` VALUES (6, 1928.59, -1771.09, 13.1728, 0, 0, 90, 0, 0);
INSERT INTO `atms` VALUES (7, 2323.77, -1644.99, 14.4427, 0, 0, 0, 0, 0);
INSERT INTO `atms` VALUES (8, 2043.75, -1416.7, 16.8108, 0, 0, -90, 0, 0);
INSERT INTO `atms` VALUES (9, 2387.75, -1981.96, 13.1569, 0, 0, -180, 0, 0);
INSERT INTO `atms` VALUES (10, 1494.45, -1768.98, 18.3657, 0, 0, -90, 0, 0);
INSERT INTO `atms` VALUES (11, 1051.63, -1026.41, 31.6616, 0, 0, 0, 0, 0);
INSERT INTO `atms` VALUES (12, 816.873, -1356.52, 13.1561, 0, 0, -180, 0, 0);
INSERT INTO `atms` VALUES (13, 1808.73, -1567.27, 13.064, 0, 0, 37, 0, 0);
INSERT INTO `atms` VALUES (14, 2412.54, -1492.67, 23.6281, 0, 0, -180, 0, 0);
INSERT INTO `atms` VALUES (15, 2431.13, -1219.48, 25.0222, 0, 0, 0, 0, 0);
INSERT INTO `atms` VALUES (16, 255.455, -197.585, 1.23812, 0, 0, -90, 0, 0);

-- ----------------------------
-- Table structure for attachments
-- ----------------------------
DROP TABLE IF EXISTS `attachments`;
CREATE TABLE `attachments`  (
  `u_id` int NULL DEFAULT NULL,
  `slot` tinyint NOT NULL,
  `model` smallint NOT NULL,
  `bone` tinyint NOT NULL,
  `offset_x` float NOT NULL,
  `offset_y` float NOT NULL,
  `offset_z` float NOT NULL,
  `rotation_x` float NOT NULL,
  `rotation_y` float NOT NULL,
  `rotation_z` float NOT NULL,
  `scale_x` float NOT NULL,
  `scale_y` float NOT NULL,
  `scale_z` float NOT NULL,
  INDEX `u_id`(`u_id` ASC) USING BTREE,
  CONSTRAINT `attachments_ibfk_1` FOREIGN KEY (`u_id`) REFERENCES `players` (`u_id`) ON DELETE CASCADE ON UPDATE RESTRICT
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of attachments
-- ----------------------------

-- ----------------------------
-- Table structure for bans
-- ----------------------------
DROP TABLE IF EXISTS `bans`;
CREATE TABLE `bans`  (
  `username` varchar(24) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL,
  `ip` varchar(17) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL,
  `gcpi` varchar(60) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL,
  `reason` varchar(32) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL,
  `admin` varchar(24) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL,
  `ban_date` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `unban_date` datetime NULL DEFAULT NULL,
  UNIQUE INDEX `username`(`username` ASC) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of bans
-- ----------------------------

-- ----------------------------
-- Table structure for biz_licenses
-- ----------------------------
DROP TABLE IF EXISTS `biz_licenses`;
CREATE TABLE `biz_licenses`  (
  `id` int NOT NULL AUTO_INCREMENT,
  `u_id` int NOT NULL,
  `biz_name` varchar(64) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL,
  `registered_at` datetime NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`) USING BTREE,
  INDEX `u_id`(`u_id` ASC) USING BTREE,
  CONSTRAINT `biz_licenses_ibfk_1` FOREIGN KEY (`u_id`) REFERENCES `players` (`u_id`) ON DELETE CASCADE ON UPDATE RESTRICT
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of biz_licenses
-- ----------------------------

-- ----------------------------
-- Table structure for businesses
-- ----------------------------
DROP TABLE IF EXISTS `businesses`;
CREATE TABLE `businesses`  (
  `id` int NOT NULL AUTO_INCREMENT,
  `owner_uid` int NULL DEFAULT 0,
  `name` varchar(64) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT 'Doanh nghiep vo chu',
  `type` int NULL DEFAULT 1,
  `price` int NULL DEFAULT 50000,
  `safe_money` int NULL DEFAULT 0,
  `stock_level` int NULL DEFAULT 1000,
  `pos_x` float NULL DEFAULT NULL,
  `pos_y` float NULL DEFAULT NULL,
  `pos_z` float NULL DEFAULT NULL,
  `vw` int NULL DEFAULT 0,
  `interior` int NULL DEFAULT 0,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of businesses
-- ----------------------------

-- ----------------------------
-- Table structure for characters
-- ----------------------------
DROP TABLE IF EXISTS `characters`;
CREATE TABLE `characters`  (
  `char_id` int NOT NULL AUTO_INCREMENT,
  `u_id` int NOT NULL COMMENT 'Foreign key -> players.u_id',
  `firstname` varchar(32) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL DEFAULT '' COMMENT 'Ten IC (Firstname)',
  `lastname` varchar(32) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL DEFAULT '' COMMENT 'Ho IC (Lastname)',
  `age` int NOT NULL DEFAULT 20,
  `gender` tinyint(1) NOT NULL DEFAULT 0 COMMENT '0=Nam, 1=Nu',
  `skin_id` int NOT NULL DEFAULT 0,
  `money` int NOT NULL DEFAULT 5000 COMMENT 'Tien mat ban dau',
  `bank_money` int NOT NULL DEFAULT 0,
  `kills` int NOT NULL DEFAULT 0,
  `deaths` int NOT NULL DEFAULT 0,
  `job_id` int NOT NULL DEFAULT 0,
  `class_id` tinyint NOT NULL DEFAULT 0,
  `wanted_level` int NOT NULL DEFAULT 0,
  `score` int NOT NULL DEFAULT 0,
  `xp` int NOT NULL DEFAULT 0,
  `pos_x` float NOT NULL DEFAULT 1543.4 COMMENT 'Spawn mac dinh: LS Hospital',
  `pos_y` float NOT NULL DEFAULT -1675.6,
  `pos_z` float NOT NULL DEFAULT 13.5,
  `pos_a` float NOT NULL DEFAULT 180,
  `created_at` datetime NULL DEFAULT CURRENT_TIMESTAMP,
  `last_played` datetime NULL DEFAULT NULL,
  PRIMARY KEY (`char_id`) USING BTREE,
  UNIQUE INDEX `unique_fullname`(`firstname` ASC, `lastname` ASC) USING BTREE,
  INDEX `u_id`(`u_id` ASC) USING BTREE,
  CONSTRAINT `characters_ibfk_1` FOREIGN KEY (`u_id`) REFERENCES `players` (`u_id`) ON DELETE CASCADE ON UPDATE RESTRICT
) ENGINE = InnoDB AUTO_INCREMENT = 5 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of characters
-- ----------------------------
INSERT INTO `characters` VALUES (2, 4, 'Jey', 'Bee', 18, 0, 1, 4520, 0, 0, 0, 0, 0, 0, 0, 0, 1530.82, -1687.29, 13.3828, 138.567, '2026-07-29 23:26:02', '2026-08-17 01:32:39');
INSERT INTO `characters` VALUES (3, 4, 'Young', 'Smith', 20, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1440.89, -1699.47, 13.5469, 182.434, '2026-07-29 23:37:22', '2026-08-16 01:24:16');
INSERT INTO `characters` VALUES (4, 4, 'Duma', 'May', 18, 0, 2, 0, 0, 0, 5, 0, 0, 0, 0, 0, 2.9484, -0.7274, -3.8288, 344.355, '2026-07-30 00:28:01', '2026-08-05 04:00:50');

-- ----------------------------
-- Table structure for custom_weapons
-- ----------------------------
DROP TABLE IF EXISTS `custom_weapons`;
CREATE TABLE `custom_weapons`  (
  `id` int NOT NULL AUTO_INCREMENT,
  `name` varchar(64) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  `base_weapon_id` int NOT NULL,
  `damage_override` float NOT NULL DEFAULT 25,
  `tier` varchar(32) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL DEFAULT 'Common',
  `max_ammo` int NOT NULL DEFAULT 250,
  `price` int NOT NULL DEFAULT 5000,
  `description` varchar(128) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL DEFAULT '',
  `enabled` tinyint(1) NOT NULL DEFAULT 1,
  `created_at` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 9 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of custom_weapons
-- ----------------------------
INSERT INTO `custom_weapons` VALUES (1, 'Desert Eagle Chrome', 24, 55, 'Epic', 200, 15000, 'Sung ngan Deagle ma chrome sang bong voi sat thuong cao.', 1, '2026-08-01 00:22:44');
INSERT INTO `custom_weapons` VALUES (2, 'AK-47 Dragon', 30, 42, 'Legendary', 500, 35000, 'Sung truong AK-47 kham hoa van Rong vang ruc ro.', 1, '2026-08-01 00:22:44');
INSERT INTO `custom_weapons` VALUES (3, 'M4A1 VIP', 31, 38, 'Mythic', 600, 50000, 'Sung M4A1 VIP phien ban dac biet toc do ban va sat thuong cuc cao.', 1, '2026-08-01 00:22:44');
INSERT INTO `custom_weapons` VALUES (4, 'Sniper Cobra', 34, 120, 'Legendary', 100, 60000, 'Sung ban tia Snipe Rắn Độc gay sat thuong chi mang.', 1, '2026-08-01 00:22:44');
INSERT INTO `custom_weapons` VALUES (5, 'Shotgun Tactical', 25, 45, 'Rare', 150, 12000, 'Sung san Shotgun bien the quan dung sat thuong dien rong.', 1, '2026-08-01 00:22:44');
INSERT INTO `custom_weapons` VALUES (6, 'MP5 SpecOps', 29, 28, 'Rare', 400, 10000, 'Sung tieu lien MP5 danh cho luc luong dac nhiem.', 1, '2026-08-01 00:22:44');
INSERT INTO `custom_weapons` VALUES (7, 'Katana Shadow', 8, 75, 'Epic', 1, 8000, 'Kiem Katana Bong Toi chem xuyen giap.', 1, '2026-08-01 00:22:44');
INSERT INTO `custom_weapons` VALUES (8, 'Combat Shotgun Enforcer', 27, 50, 'Mythic', 250, 45000, 'Shotgun tu dong Enforcer voi uy luc ap đảo.', 1, '2026-08-01 00:22:44');

-- ----------------------------
-- Table structure for doors
-- ----------------------------
DROP TABLE IF EXISTS `doors`;
CREATE TABLE `doors`  (
  `id` int NOT NULL AUTO_INCREMENT,
  `name` varchar(64) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  `slug` varchar(64) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL DEFAULT '',
  `label` varchar(128) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL DEFAULT '',
  `exterior_x` float NOT NULL DEFAULT 0,
  `exterior_y` float NOT NULL DEFAULT 0,
  `exterior_z` float NOT NULL DEFAULT 0,
  `exterior_a` float NOT NULL DEFAULT 0,
  `exterior_interior` int NOT NULL DEFAULT 0,
  `exterior_vw` int NOT NULL DEFAULT 0,
  `interior_x` float NOT NULL DEFAULT 0,
  `interior_y` float NOT NULL DEFAULT 0,
  `interior_z` float NOT NULL DEFAULT 0,
  `interior_a` float NOT NULL DEFAULT 0,
  `interior_interior` int NOT NULL DEFAULT 0,
  `interior_vw` int NOT NULL DEFAULT 0,
  `pickup_model` int NOT NULL DEFAULT 19130,
  `door_type` int NOT NULL DEFAULT 0,
  `locked` tinyint(1) NOT NULL DEFAULT 0,
  `faction_id` int NOT NULL DEFAULT 0,
  `business_id` int NOT NULL DEFAULT 0,
  `house_id` int NOT NULL DEFAULT 0,
  `admin_level` int NOT NULL DEFAULT 0,
  `vip_level` int NOT NULL DEFAULT 0,
  `enabled` tinyint(1) NOT NULL DEFAULT 1,
  `created_by` varchar(32) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL DEFAULT 'System',
  `created_at` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `updated_at` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 37 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of doors
-- ----------------------------
INSERT INTO `doors` VALUES (1, 'PHONG GYM', 'gym', 'PHONG GYM', 2229.9, -1721.26, 13.5612, 0, 0, 0, 772.307, -5.5157, 1000.73, 0, 5, 0, 19130, 0, 0, 0, 0, 0, 0, 0, 1, 'System', '2026-08-01 00:22:43', '2026-08-01 00:22:43');
INSERT INTO `doors` VALUES (2, 'QUAN BAR', 'bar', 'QUAN BAR', 2309.99, -1643.44, 14.827, 0, 0, 0, 501.901, -67.5635, 998.758, 0, 11, 1, 19130, 0, 0, 0, 0, 0, 0, 0, 1, 'System', '2026-08-01 00:22:43', '2026-08-01 00:22:43');
INSERT INTO `doors` VALUES (3, 'CUA HANG GA RAN 1', 'chicken_1', 'CUA HANG GA RAN', 2397.82, -1899.19, 13.5469, 0, 0, 0, 364.99, -11.8441, 1001.85, 0, 9, 2, 19130, 0, 0, 0, 0, 0, 0, 0, 1, 'System', '2026-08-01 00:22:43', '2026-08-01 00:22:43');
INSERT INTO `doors` VALUES (4, 'CUA HANG GA RAN 2', 'chicken_2', 'CUA HANG GA RAN', 928.916, -1353.04, 13.3438, 0, 0, 0, 364.99, -11.8441, 1001.85, 0, 9, 3, 19130, 0, 0, 0, 0, 0, 0, 0, 1, 'System', '2026-08-01 00:22:43', '2026-08-01 00:22:43');
INSERT INTO `doors` VALUES (5, 'CUA HANG GA RAN 3', 'chicken_3', 'CUA HANG GA RAN', 2419.7, -1509.05, 24, 0, 0, 0, 364.99, -11.8441, 1001.85, 0, 9, 4, 19130, 0, 0, 0, 0, 0, 0, 0, 1, 'System', '2026-08-01 00:22:43', '2026-08-01 00:22:43');
INSERT INTO `doors` VALUES (6, 'BINCO', 'binco', 'BINCO', 2244.38, -1665.57, 15.4766, 0, 0, 0, 207.667, -111.266, 1005.13, 0, 15, 5, 19130, 0, 0, 0, 0, 0, 0, 0, 1, 'System', '2026-08-01 00:22:43', '2026-08-01 00:22:43');
INSERT INTO `doors` VALUES (7, 'CUA HANG VU KHI 1', 'weapon_1', 'CUA HANG VU KHI', 2400.49, -1982, 13.5469, 0, 0, 0, 285.856, -86.782, 1001.52, 0, 4, 6, 19130, 0, 0, 0, 0, 0, 0, 0, 1, 'System', '2026-08-01 00:22:43', '2026-08-01 00:22:43');
INSERT INTO `doors` VALUES (8, 'CUA HANG VU KHI 2', 'weapon_2', 'CUA HANG VU KHI', 1369, -1279.71, 13.5469, 0, 0, 0, 316.349, -170.297, 999.594, 0, 6, 7, 19130, 0, 0, 0, 0, 0, 0, 0, 1, 'System', '2026-08-01 00:22:43', '2026-08-01 00:22:43');
INSERT INTO `doors` VALUES (9, 'CUA HANG NGUOI LON 1', 'sexshop_1', 'CUA HANG NGUOI LON', 1940.01, -2115.98, 13.6953, 0, 0, 0, -100.356, -25.0387, 1000.72, 0, 3, 8, 19130, 0, 0, 0, 0, 0, 0, 0, 1, 'System', '2026-08-01 00:22:43', '2026-08-01 00:22:43');
INSERT INTO `doors` VALUES (10, 'CUA HANG NGUOI LON 2', 'sexshop_2', 'CUA HANG NGUOI LON', 1087.68, -922.482, 43.3906, 0, 0, 0, -100.356, -25.0387, 1000.72, 0, 3, 9, 19130, 0, 0, 0, 0, 0, 0, 0, 1, 'System', '2026-08-01 00:22:43', '2026-08-01 00:22:43');
INSERT INTO `doors` VALUES (11, 'TRAM XANG 1', 'gas_1', 'TRAM XANG', 1928.58, -1776.26, 13.5469, 0, 0, 0, -27.2923, -58.0535, 1003.55, 0, 6, 10, 19130, 0, 0, 0, 0, 0, 0, 0, 1, 'System', '2026-08-01 00:22:43', '2026-08-01 00:22:43');
INSERT INTO `doors` VALUES (12, 'TRAM XANG 2', 'gas_2', 'TRAM XANG', -78.3609, -1169.87, 2.1355, 0, 0, 0, -27.2923, -58.0535, 1003.55, 0, 6, 11, 19130, 0, 0, 0, 0, 0, 0, 0, 1, 'System', '2026-08-01 00:22:43', '2026-08-01 00:22:43');
INSERT INTO `doors` VALUES (13, 'CUA HANG BURGER 1', 'burger_1', 'CUA HANG BURGER', 810.485, -1616.13, 13.5469, 0, 0, 0, 362.882, -75.1634, 1001.51, 0, 10, 12, 19130, 0, 0, 0, 0, 0, 0, 0, 1, 'System', '2026-08-01 00:22:43', '2026-08-01 00:22:43');
INSERT INTO `doors` VALUES (14, 'CUA HANG BURGER 2', 'burger_2', 'CUA HANG BURGER', 1199.26, -918.142, 43.1232, 0, 0, 0, 362.882, -75.1634, 1001.51, 0, 10, 13, 19130, 0, 0, 0, 0, 0, 0, 0, 1, 'System', '2026-08-01 00:22:43', '2026-08-01 00:22:43');
INSERT INTO `doors` VALUES (15, 'TIEM TOC 1', 'barber_1', 'TIEM TOC', 824.06, -1588.32, 13.5436, 0, 0, 0, 411.892, -54.4434, 1001.9, 0, 12, 14, 19130, 0, 0, 0, 0, 0, 0, 0, 1, 'System', '2026-08-01 00:22:43', '2026-08-01 00:22:43');
INSERT INTO `doors` VALUES (16, 'TIEM TOC 2', 'barber_2', 'TIEM TOC', 2070.63, -1793.84, 13.5469, 0, 0, 0, 411.892, -54.4434, 1001.9, 0, 12, 15, 19130, 0, 0, 0, 0, 0, 0, 0, 1, 'System', '2026-08-01 00:22:43', '2026-08-01 00:22:43');
INSERT INTO `doors` VALUES (17, 'TRUNG TAM DICH VU', 'service_center', 'TRUNG TAM DICH VU', 1555.5, -1675.64, 16.1953, 0, 0, 0, 246.837, 62.3343, 1003.64, 0, 6, 16, 19130, 0, 0, 0, 0, 0, 0, 0, 1, 'System', '2026-08-01 00:22:43', '2026-08-01 00:22:43');
INSERT INTO `doors` VALUES (18, 'TIEM PIZZA', 'pizza', 'TIEM PIZZA', 2105.49, -1806.57, 13.5547, 0, 0, 0, 372.274, -133.525, 1001.49, 0, 5, 17, 19130, 0, 0, 0, 0, 0, 0, 0, 1, 'System', '2026-08-01 00:22:43', '2026-08-01 00:22:43');
INSERT INTO `doors` VALUES (19, 'CAU LAC BO DEM', 'nightclub', 'CAU LAC BO DEM', 2421.6, -1219.24, 25.5614, 0, 0, 0, 1204.76, -13.8523, 1000.92, 0, 2, 18, 19130, 0, 0, 0, 0, 0, 0, 0, 1, 'System', '2026-08-01 00:22:43', '2026-08-01 00:22:43');
INSERT INTO `doors` VALUES (20, 'SAN NHAY', 'danceclub', 'SAN NHAY', 1837.04, -1682.4, 13.3229, 0, 0, 0, 493.481, -24.9531, 1000.67, 0, 17, 19, 19130, 0, 0, 0, 0, 0, 0, 0, 1, 'System', '2026-08-01 00:22:43', '2026-08-01 00:22:43');
INSERT INTO `doors` VALUES (21, '24/7 1', 'store247_1', '24/7', 1833.78, -1842.62, 13.5781, 0, 0, 0, -25.9472, -188.26, 1003.55, 0, 17, 20, 19130, 0, 0, 0, 0, 0, 0, 0, 1, 'System', '2026-08-01 00:22:43', '2026-08-01 00:22:43');
INSERT INTO `doors` VALUES (22, '24/7 2', 'store247_2', '24/7', 1000.59, -919.917, 42.3281, 0, 0, 0, -25.9472, -188.26, 1003.55, 0, 17, 21, 19130, 0, 0, 0, 0, 0, 0, 0, 1, 'System', '2026-08-01 00:22:43', '2026-08-01 00:22:43');
INSERT INTO `doors` VALUES (23, 'TIEM XAM HINH', 'tattoo', 'TIEM XAM HINH', 2068.58, -1779.85, 13.5596, 0, 0, 0, -204.417, -27.347, 1002.27, 0, 16, 22, 19130, 0, 0, 0, 0, 0, 0, 0, 1, 'System', '2026-08-01 00:22:43', '2026-08-01 00:22:43');
INSERT INTO `doors` VALUES (24, 'CUA HANG THOI TRANG 1', 'clothes_1', 'CUA HANG THOI TRANG', 2112.86, -1211.45, 23.9629, 0, 0, 0, 203.841, -50.6566, 1001.8, 0, 1, 23, 19130, 0, 0, 0, 0, 0, 0, 0, 1, 'System', '2026-08-01 00:22:43', '2026-08-01 00:22:43');
INSERT INTO `doors` VALUES (25, 'CUA HANG THOI TRANG 2', 'clothes_2', 'CUA HANG THOI TRANG', 461.707, -1500.85, 31.0449, 0, 0, 0, 227.368, -8.3722, 1002.21, 0, 5, 24, 19130, 0, 0, 0, 0, 0, 0, 0, 1, 'System', '2026-08-01 00:22:43', '2026-08-01 00:22:43');
INSERT INTO `doors` VALUES (26, 'TIEM BANH', 'bakery', 'TIEM BANH', 1038.1, -1340.73, 13.745, 0, 0, 0, 377.131, -193.305, 1000.63, 0, 17, 25, 19130, 0, 0, 0, 0, 0, 0, 0, 1, 'System', '2026-08-01 00:22:43', '2026-08-01 00:22:43');
INSERT INTO `doors` VALUES (27, 'TOA THI CHINH', 'cityhall', 'TOA THI CHINH', 1481.04, -1772.31, 18.7958, 0, 0, 0, 390.746, 173.763, 1008.38, 0, 3, 26, 19130, 0, 0, 0, 0, 0, 0, 0, 1, 'System', '2026-08-01 00:22:43', '2026-08-01 00:22:43');
INSERT INTO `doors` VALUES (28, 'KHACH SAN JEFFERSON', 'hotel_jefferson', 'KHACH SAN JEFFERSON', 2233.29, -1159.85, 25.8906, 0, 0, 0, 2214.38, -1150.48, 1025.8, 0, 15, 27, 19130, 0, 0, 0, 0, 0, 0, 0, 1, 'System', '2026-08-01 00:22:43', '2026-08-01 00:22:43');
INSERT INTO `doors` VALUES (29, 'CUA HANG THE THAO', 'suburban', 'CUA HANG THE THAO', 499.535, -1360.63, 16.369, 0, 0, 0, 207.025, -140.376, 1003.51, 0, 3, 28, 19130, 0, 0, 0, 0, 0, 0, 0, 1, 'System', '2026-08-01 00:22:43', '2026-08-01 00:22:43');
INSERT INTO `doors` VALUES (30, 'CUA HANG THOI TRANG 3', 'victim', 'CUA HANG THOI TRANG', 1457.07, -1137.1, 23.9441, 0, 0, 0, 161.298, -97.1033, 1001.8, 0, 18, 29, 19130, 0, 0, 0, 0, 0, 0, 0, 1, 'System', '2026-08-01 00:22:43', '2026-08-01 00:22:43');
INSERT INTO `doors` VALUES (31, 'NGAN HANG', 'bank', 'NGAN HANG', 595.665, -1250.79, 18.2999, 0, 0, 0, 2306.16, -16.2888, 26.7496, 0, 0, 30, 19130, 0, 0, 0, 0, 0, 0, 0, 1, 'System', '2026-08-01 00:22:43', '2026-08-01 00:22:43');
INSERT INTO `doors` VALUES (32, 'SONG BAC', 'casino', 'SONG BAC', 1022.65, -1121.9, 23.8719, 0, 0, 0, 1133.21, -15.363, 1000.68, 0, 12, 31, 19130, 0, 0, 0, 0, 0, 0, 0, 1, 'System', '2026-08-01 00:22:43', '2026-08-01 00:22:43');
INSERT INTO `doors` VALUES (33, 'BIET THU', 'mansion', 'BIET THU', 1298.3, -798.774, 84.1406, 0, 0, 0, 1299.04, -796.724, 1084.01, 0, 5, 32, 19130, 0, 0, 0, 0, 0, 0, 0, 1, 'System', '2026-08-01 00:22:43', '2026-08-01 00:22:43');
INSERT INTO `doors` VALUES (34, 'DAI LY XE', 'dealership', 'DAI LY XE', 541.963, -1293.52, 17.2414, 0, 0, 0, 849.423, -973.605, 1090.1, 0, 1, 33, 19130, 0, 0, 0, 0, 0, 0, 0, 1, 'System', '2026-08-01 00:22:43', '2026-08-01 00:22:43');
INSERT INTO `doors` VALUES (35, 'NHA KHO', 'warehouse', 'NHA KHO', 2166.42, -1671.77, 15.0742, 0, 0, 0, 742.977, 207.161, 11558.8, 0, 1, 34, 19130, 0, 0, 0, 0, 0, 0, 0, 1, 'System', '2026-08-01 00:22:43', '2026-08-01 00:22:43');
INSERT INTO `doors` VALUES (36, 'TOA THAP', 'tower', 'TOA THAP', 1807.07, -1290.11, 131.734, 90, 0, 0, 1815.95, -1317.69, 125.727, 90, 0, 35, 19130, 0, 0, 0, 0, 0, 0, 0, 1, 'System', '2026-08-01 00:22:43', '2026-08-01 00:22:43');

-- ----------------------------
-- Table structure for entrances
-- ----------------------------
DROP TABLE IF EXISTS `entrances`;
CREATE TABLE `entrances`  (
  `entrance_id` int NOT NULL AUTO_INCREMENT,
  `name` varchar(30) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL DEFAULT 'LOI VAO',
  `map_icon` smallint NOT NULL DEFAULT 0,
  `freeze_player` tinyint(1) NOT NULL DEFAULT 0,
  `entry_x` float NOT NULL,
  `entry_y` float NOT NULL,
  `entry_z` float NOT NULL,
  `entry_a` float NOT NULL DEFAULT 0,
  `entry_world` int NOT NULL DEFAULT 0,
  `entry_interior` int NOT NULL DEFAULT 0,
  `exit_x` float NOT NULL,
  `exit_y` float NOT NULL,
  `exit_z` float NOT NULL,
  `exit_a` float NOT NULL DEFAULT 0,
  `exit_world` int NOT NULL DEFAULT 0,
  `exit_interior` int NOT NULL DEFAULT 0,
  `enabled` tinyint(1) NOT NULL DEFAULT 1,
  `created_at` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `updated_at` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`entrance_id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 5 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of entrances
-- ----------------------------
INSERT INTO `entrances` VALUES (1, 'PHONG GYM', 54, 0, -2016.94, 216.816, 27.7916, 239.228, 0, 0, 772.307, -5.5157, 1000.73, 0, 1000, 5, 1, '2026-07-28 00:31:13', '2026-07-28 00:31:13');
INSERT INTO `entrances` VALUES (2, 'QUAN BAR', 49, 0, -2017.14, 206.194, 27.872, 167.474, 0, 0, 501.901, -67.5635, 998.758, 0, 1001, 11, 1, '2026-07-28 00:31:23', '2026-07-28 00:31:23');
INSERT INTO `entrances` VALUES (3, 'CUA HANG GA RAN', 14, 0, -2010.81, 199.479, 27.5391, 233.588, 0, 0, 364.99, -11.8441, 1001.85, 0, 1002, 9, 1, '2026-07-28 00:31:28', '2026-07-28 00:31:28');
INSERT INTO `entrances` VALUES (4, 'TRAM XANG', 55, 0, -2008.65, 198.179, 27.5391, 243.06, 0, 0, -27.2923, -58.0535, 1003.55, 0, 1003, 6, 1, '2026-07-28 00:31:56', '2026-07-28 00:31:56');

-- ----------------------------
-- Table structure for faction_inventory
-- ----------------------------
DROP TABLE IF EXISTS `faction_inventory`;
CREATE TABLE `faction_inventory`  (
  `id` int NOT NULL AUTO_INCREMENT,
  `faction_id` int NOT NULL,
  `item_name` varchar(32) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL,
  `item_label` varchar(32) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL,
  `quantity` int NOT NULL DEFAULT 1,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of faction_inventory
-- ----------------------------

-- ----------------------------
-- Table structure for gang_members
-- ----------------------------
DROP TABLE IF EXISTS `gang_members`;
CREATE TABLE `gang_members`  (
  `gang_id` int NOT NULL AUTO_INCREMENT,
  `user_id` int NOT NULL,
  `gang_rank` int NOT NULL DEFAULT 1,
  PRIMARY KEY (`user_id`) USING BTREE,
  INDEX `gang_id`(`gang_id` ASC) USING BTREE,
  CONSTRAINT `gang_members_ibfk_1` FOREIGN KEY (`user_id`) REFERENCES `players` (`u_id`) ON DELETE RESTRICT ON UPDATE RESTRICT,
  CONSTRAINT `gang_members_ibfk_2` FOREIGN KEY (`gang_id`) REFERENCES `gangs` (`id`) ON DELETE CASCADE ON UPDATE RESTRICT
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of gang_members
-- ----------------------------

-- ----------------------------
-- Table structure for gangs
-- ----------------------------
DROP TABLE IF EXISTS `gangs`;
CREATE TABLE `gangs`  (
  `id` int NOT NULL AUTO_INCREMENT,
  `leader_uid` int NOT NULL,
  `name` varchar(30) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL,
  `colour` bigint NOT NULL DEFAULT -1,
  `kills` int NOT NULL DEFAULT 0,
  `deaths` int NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`) USING BTREE,
  INDEX `leader_uid`(`leader_uid` ASC) USING BTREE,
  CONSTRAINT `gangs_ibfk_1` FOREIGN KEY (`leader_uid`) REFERENCES `players` (`u_id`) ON DELETE CASCADE ON UPDATE RESTRICT
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of gangs
-- ----------------------------

-- ----------------------------
-- Table structure for house_furniture
-- ----------------------------
DROP TABLE IF EXISTS `house_furniture`;
CREATE TABLE `house_furniture`  (
  `furniture_id` int NOT NULL AUTO_INCREMENT,
  `house_id` smallint NOT NULL,
  `object_id` int NOT NULL,
  `x` float NOT NULL,
  `y` float NOT NULL,
  `z` float NOT NULL,
  `rx` float NOT NULL,
  `ry` float NOT NULL,
  `rz` float NOT NULL,
  `intid` int NOT NULL,
  `vworld` int NOT NULL,
  PRIMARY KEY (`furniture_id`) USING BTREE,
  INDEX `house_id`(`house_id` ASC) USING BTREE,
  CONSTRAINT `house_furniture_ibfk_1` FOREIGN KEY (`house_id`) REFERENCES `player_houses` (`house_id`) ON DELETE CASCADE ON UPDATE RESTRICT
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of house_furniture
-- ----------------------------

-- ----------------------------
-- Table structure for house_inventory
-- ----------------------------
DROP TABLE IF EXISTS `house_inventory`;
CREATE TABLE `house_inventory`  (
  `id` int NOT NULL AUTO_INCREMENT,
  `house_id` smallint NOT NULL,
  `item_name` varchar(32) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL,
  `item_label` varchar(32) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL,
  `quantity` int NOT NULL DEFAULT 1,
  PRIMARY KEY (`id`) USING BTREE,
  INDEX `house_id`(`house_id` ASC) USING BTREE,
  CONSTRAINT `house_inventory_ibfk_1` FOREIGN KEY (`house_id`) REFERENCES `player_houses` (`house_id`) ON DELETE CASCADE ON UPDATE RESTRICT
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of house_inventory
-- ----------------------------

-- ----------------------------
-- Table structure for house_key_holders
-- ----------------------------
DROP TABLE IF EXISTS `house_key_holders`;
CREATE TABLE `house_key_holders`  (
  `houseid` smallint NOT NULL,
  `uid` int NOT NULL,
  PRIMARY KEY (`houseid`) USING BTREE,
  INDEX `uid`(`uid` ASC) USING BTREE,
  CONSTRAINT `house_key_holders_ibfk_1` FOREIGN KEY (`houseid`) REFERENCES `player_houses` (`house_id`) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT `house_key_holders_ibfk_2` FOREIGN KEY (`uid`) REFERENCES `players` (`u_id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of house_key_holders
-- ----------------------------

-- ----------------------------
-- Table structure for house_settings
-- ----------------------------
DROP TABLE IF EXISTS `house_settings`;
CREATE TABLE `house_settings`  (
  `house_id` smallint NOT NULL,
  `locked` tinyint(1) NOT NULL DEFAULT 0,
  PRIMARY KEY (`house_id`) USING BTREE,
  CONSTRAINT `house_settings_ibfk_1` FOREIGN KEY (`house_id`) REFERENCES `player_houses` (`house_id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of house_settings
-- ----------------------------

-- ----------------------------
-- Table structure for jailed
-- ----------------------------
DROP TABLE IF EXISTS `jailed`;
CREATE TABLE `jailed`  (
  `u_id` int NOT NULL AUTO_INCREMENT,
  `bail_required` tinyint NOT NULL DEFAULT 0,
  `jailed_time` mediumint NOT NULL DEFAULT 0,
  `bail_balance` mediumint NOT NULL DEFAULT 0,
  PRIMARY KEY (`u_id`) USING BTREE,
  CONSTRAINT `jailed_ibfk_1` FOREIGN KEY (`u_id`) REFERENCES `players` (`u_id`) ON DELETE CASCADE ON UPDATE RESTRICT
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of jailed
-- ----------------------------

-- ----------------------------
-- Table structure for job_skills
-- ----------------------------
DROP TABLE IF EXISTS `job_skills`;
CREATE TABLE `job_skills`  (
  `id` int NOT NULL AUTO_INCREMENT,
  `char_id` int NOT NULL DEFAULT 0,
  `job_id` tinyint NOT NULL DEFAULT 0,
  `xp` int NOT NULL DEFAULT 0,
  `level` tinyint NOT NULL DEFAULT 1,
  PRIMARY KEY (`id`) USING BTREE,
  UNIQUE INDEX `char_job`(`char_id` ASC, `job_id` ASC) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 121 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of job_skills
-- ----------------------------
INSERT INTO `job_skills` VALUES (1, 2, 2, 0, 1);
INSERT INTO `job_skills` VALUES (2, 2, 1, 0, 1);
INSERT INTO `job_skills` VALUES (3, 2, 4, 0, 1);
INSERT INTO `job_skills` VALUES (4, 2, 3, 0, 1);
INSERT INTO `job_skills` VALUES (5, 2, 5, 0, 1);
INSERT INTO `job_skills` VALUES (6, 0, 1, 0, 1);
INSERT INTO `job_skills` VALUES (7, 0, 2, 0, 1);
INSERT INTO `job_skills` VALUES (8, 0, 3, 0, 1);
INSERT INTO `job_skills` VALUES (9, 0, 4, 0, 1);
INSERT INTO `job_skills` VALUES (10, 0, 5, 0, 1);

-- ----------------------------
-- Table structure for phone_contacts
-- ----------------------------
DROP TABLE IF EXISTS `phone_contacts`;
CREATE TABLE `phone_contacts`  (
  `id` int NOT NULL AUTO_INCREMENT,
  `owner_uid` int NOT NULL,
  `name` varchar(32) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT 'Ten luu trong danh ba',
  `number` varchar(10) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL,
  PRIMARY KEY (`id`) USING BTREE,
  INDEX `owner_uid`(`owner_uid` ASC) USING BTREE,
  CONSTRAINT `phone_contacts_ibfk_1` FOREIGN KEY (`owner_uid`) REFERENCES `players` (`u_id`) ON DELETE CASCADE ON UPDATE RESTRICT
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of phone_contacts
-- ----------------------------

-- ----------------------------
-- Table structure for phone_numbers
-- ----------------------------
DROP TABLE IF EXISTS `phone_numbers`;
CREATE TABLE `phone_numbers`  (
  `u_id` int NOT NULL,
  `phone_num` varchar(10) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT 'So dien thoai 7 chu so (dau 09x...)',
  PRIMARY KEY (`u_id`) USING BTREE,
  UNIQUE INDEX `phone_num`(`phone_num` ASC) USING BTREE,
  CONSTRAINT `phone_numbers_ibfk_1` FOREIGN KEY (`u_id`) REFERENCES `players` (`u_id`) ON DELETE CASCADE ON UPDATE RESTRICT
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of phone_numbers
-- ----------------------------
INSERT INTO `phone_numbers` VALUES (4, '09426929');

-- ----------------------------
-- Table structure for player_bank
-- ----------------------------
DROP TABLE IF EXISTS `player_bank`;
CREATE TABLE `player_bank`  (
  `u_id` int NOT NULL,
  `money` int NOT NULL DEFAULT 0,
  PRIMARY KEY (`u_id`) USING BTREE,
  CONSTRAINT `player_bank_ibfk_1` FOREIGN KEY (`u_id`) REFERENCES `players` (`u_id`) ON DELETE CASCADE ON UPDATE RESTRICT
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of player_bank
-- ----------------------------
INSERT INTO `player_bank` VALUES (2, 0);
INSERT INTO `player_bank` VALUES (3, 0);

-- ----------------------------
-- Table structure for player_custom_weapons
-- ----------------------------
DROP TABLE IF EXISTS `player_custom_weapons`;
CREATE TABLE `player_custom_weapons`  (
  `id` int NOT NULL AUTO_INCREMENT,
  `u_id` int NOT NULL,
  `weapon_def_id` int NOT NULL,
  `ammo` int NOT NULL DEFAULT 100,
  `durability` float NOT NULL DEFAULT 100,
  `is_equipped` tinyint(1) NOT NULL DEFAULT 0,
  `obtained_at` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`) USING BTREE,
  INDEX `weapon_def_id`(`weapon_def_id` ASC) USING BTREE,
  CONSTRAINT `player_custom_weapons_ibfk_1` FOREIGN KEY (`weapon_def_id`) REFERENCES `custom_weapons` (`id`) ON DELETE CASCADE ON UPDATE RESTRICT
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of player_custom_weapons
-- ----------------------------

-- ----------------------------
-- Table structure for player_hits
-- ----------------------------
DROP TABLE IF EXISTS `player_hits`;
CREATE TABLE `player_hits`  (
  `u_id` int NOT NULL,
  `amount` int NOT NULL DEFAULT 0,
  `reason` varchar(32) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL DEFAULT '',
  `placed_by` varchar(24) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL DEFAULT '',
  `created_at` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`u_id`) USING BTREE,
  CONSTRAINT `player_hits_ibfk_1` FOREIGN KEY (`u_id`) REFERENCES `players` (`u_id`) ON DELETE CASCADE ON UPDATE RESTRICT
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of player_hits
-- ----------------------------

-- ----------------------------
-- Table structure for player_houses
-- ----------------------------
DROP TABLE IF EXISTS `player_houses`;
CREATE TABLE `player_houses`  (
  `house_id` smallint NOT NULL DEFAULT 0,
  `u_id` int NULL DEFAULT NULL,
  `description` varchar(32) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT 'No description',
  `price` mediumint NOT NULL,
  `money_stored` mediumint NOT NULL,
  `interior` tinyint NOT NULL DEFAULT 0,
  `entrance_x` float NOT NULL,
  `entrance_y` float NOT NULL,
  `entrance_z` float NOT NULL,
  `entrance_a` float NOT NULL,
  `interior_id` tinyint NOT NULL,
  `virtual_world` mediumint NOT NULL,
  UNIQUE INDEX `house_id`(`house_id` ASC) USING BTREE,
  INDEX `u_id`(`u_id` ASC) USING BTREE,
  CONSTRAINT `player_houses_ibfk_1` FOREIGN KEY (`u_id`) REFERENCES `players` (`u_id`) ON DELETE SET NULL ON UPDATE RESTRICT
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of player_houses
-- ----------------------------

-- ----------------------------
-- Table structure for player_inventory
-- ----------------------------
DROP TABLE IF EXISTS `player_inventory`;
CREATE TABLE `player_inventory`  (
  `inv_id` int NOT NULL AUTO_INCREMENT,
  `u_id` int NOT NULL,
  `item_name` varchar(64) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT 'Ten vat pham (slug)',
  `item_label` varchar(64) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL DEFAULT '' COMMENT 'Ten hien thi',
  `quantity` int NOT NULL DEFAULT 1,
  `weight` float NOT NULL DEFAULT 0 COMMENT 'Trong luong moi don vi (kg)',
  `is_illegal` tinyint(1) NOT NULL DEFAULT 0 COMMENT '0=hop le, 1=bat hop le',
  `extra_data` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL COMMENT 'JSON cho vat pham dac biet (VD serial so pha co)',
  PRIMARY KEY (`inv_id`) USING BTREE,
  INDEX `idx_uid`(`u_id` ASC) USING BTREE,
  CONSTRAINT `player_inventory_ibfk_1` FOREIGN KEY (`u_id`) REFERENCES `players` (`u_id`) ON DELETE CASCADE ON UPDATE RESTRICT
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of player_inventory
-- ----------------------------

-- ----------------------------
-- Table structure for player_items
-- ----------------------------
DROP TABLE IF EXISTS `player_items`;
CREATE TABLE `player_items`  (
  `u_id` int NOT NULL,
  `c4` tinyint NOT NULL DEFAULT 0,
  `crack` tinyint NOT NULL DEFAULT 0,
  `weed` tinyint NOT NULL DEFAULT 0,
  `picklock` tinyint NOT NULL DEFAULT 0,
  `wallet` tinyint NOT NULL DEFAULT 0,
  `rope` tinyint NOT NULL DEFAULT 0,
  `condom` tinyint NOT NULL DEFAULT 0,
  `scissors` tinyint NOT NULL DEFAULT 0,
  `oranges` tinyint NOT NULL DEFAULT 0,
  `bottles` tinyint NOT NULL DEFAULT 0,
  `orange_juice` tinyint NOT NULL DEFAULT 0,
  `fruit_picker` tinyint NOT NULL DEFAULT 0,
  UNIQUE INDEX `u_id`(`u_id` ASC) USING BTREE,
  CONSTRAINT `player_items_ibfk_1` FOREIGN KEY (`u_id`) REFERENCES `players` (`u_id`) ON DELETE CASCADE ON UPDATE RESTRICT
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of player_items
-- ----------------------------
INSERT INTO `player_items` VALUES (1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO `player_items` VALUES (2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO `player_items` VALUES (3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO `player_items` VALUES (4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

-- ----------------------------
-- Table structure for player_job_progress
-- ----------------------------
DROP TABLE IF EXISTS `player_job_progress`;
CREATE TABLE `player_job_progress`  (
  `u_id` int NOT NULL,
  `job_id` smallint NOT NULL,
  `job_xp` int NOT NULL DEFAULT 0,
  `job_level` smallint NOT NULL DEFAULT 1,
  PRIMARY KEY (`u_id`, `job_id`) USING BTREE,
  CONSTRAINT `player_job_progress_ibfk_1` FOREIGN KEY (`u_id`) REFERENCES `players` (`u_id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of player_job_progress
-- ----------------------------

-- ----------------------------
-- Table structure for player_licenses
-- ----------------------------
DROP TABLE IF EXISTS `player_licenses`;
CREATE TABLE `player_licenses`  (
  `u_id` int NOT NULL,
  `license_id` tinyint NOT NULL,
  `issued_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`u_id`, `license_id`) USING BTREE,
  CONSTRAINT `player_licenses_ibfk_1` FOREIGN KEY (`u_id`) REFERENCES `players` (`u_id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of player_licenses
-- ----------------------------

-- ----------------------------
-- Table structure for player_phone_contacts
-- ----------------------------
DROP TABLE IF EXISTS `player_phone_contacts`;
CREATE TABLE `player_phone_contacts`  (
  `u_id` int NOT NULL,
  `contact_name` varchar(24) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL,
  `phone_number` int NOT NULL,
  PRIMARY KEY (`u_id`, `phone_number`) USING BTREE,
  CONSTRAINT `player_phone_contacts_ibfk_1` FOREIGN KEY (`u_id`) REFERENCES `players` (`u_id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of player_phone_contacts
-- ----------------------------

-- ----------------------------
-- Table structure for player_phones
-- ----------------------------
DROP TABLE IF EXISTS `player_phones`;
CREATE TABLE `player_phones`  (
  `u_id` int NOT NULL,
  `phone_number` int NOT NULL,
  `created_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`u_id`) USING BTREE,
  UNIQUE INDEX `phone_number`(`phone_number` ASC) USING BTREE,
  CONSTRAINT `player_phones_ibfk_1` FOREIGN KEY (`u_id`) REFERENCES `players` (`u_id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of player_phones
-- ----------------------------
INSERT INTO `player_phones` VALUES (2, 500002, '2026-07-27 23:44:42');
INSERT INTO `player_phones` VALUES (3, 500003, '2026-07-27 23:31:59');

-- ----------------------------
-- Table structure for player_positions
-- ----------------------------
DROP TABLE IF EXISTS `player_positions`;
CREATE TABLE `player_positions`  (
  `u_id` int NOT NULL,
  `x` float NOT NULL DEFAULT 0,
  `y` float NOT NULL DEFAULT 0,
  `z` float NOT NULL DEFAULT 0,
  `angle` float NOT NULL DEFAULT 0,
  `world` int NOT NULL DEFAULT 0,
  `interior` int NOT NULL DEFAULT 0,
  `updated_at` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`u_id`) USING BTREE,
  CONSTRAINT `player_positions_ibfk_1` FOREIGN KEY (`u_id`) REFERENCES `players` (`u_id`) ON DELETE CASCADE ON UPDATE RESTRICT
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of player_positions
-- ----------------------------
INSERT INTO `player_positions` VALUES (2, 1530.82, -1687.29, 13.3828, 138.567, 0, 0, '2026-08-17 01:32:39');

-- ----------------------------
-- Table structure for player_shots_stats
-- ----------------------------
DROP TABLE IF EXISTS `player_shots_stats`;
CREATE TABLE `player_shots_stats`  (
  `uid` int NOT NULL,
  `chest` int NOT NULL DEFAULT 0,
  `torso` int NOT NULL DEFAULT 0,
  `left_arm` int NOT NULL DEFAULT 0,
  `right_arm` int NOT NULL DEFAULT 0,
  `left_leg` int NOT NULL DEFAULT 0,
  `right_leg` int NOT NULL DEFAULT 0,
  `head` int NOT NULL DEFAULT 0,
  `misses` int NOT NULL DEFAULT 0,
  PRIMARY KEY (`uid`) USING BTREE,
  CONSTRAINT `player_shots_stats_ibfk_1` FOREIGN KEY (`uid`) REFERENCES `players` (`u_id`) ON DELETE CASCADE ON UPDATE RESTRICT
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of player_shots_stats
-- ----------------------------
INSERT INTO `player_shots_stats` VALUES (1, 0, 0, 0, 0, 0, 0, 0, 3);
INSERT INTO `player_shots_stats` VALUES (2, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO `player_shots_stats` VALUES (3, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO `player_shots_stats` VALUES (4, 0, 0, 0, 0, 0, 0, 0, 0);

-- ----------------------------
-- Table structure for player_stats
-- ----------------------------
DROP TABLE IF EXISTS `player_stats`;
CREATE TABLE `player_stats`  (
  `u_id` int NOT NULL AUTO_INCREMENT,
  `armour` float NOT NULL DEFAULT 100,
  `health` float NOT NULL DEFAULT 100,
  `kills` mediumint NOT NULL DEFAULT 0,
  `deaths` mediumint NOT NULL DEFAULT 0,
  `job_id` tinyint NOT NULL DEFAULT 0,
  `class_id` tinyint NOT NULL DEFAULT 0,
  `wanted_level` mediumint NOT NULL DEFAULT 0,
  `player_bounty` mediumint NOT NULL DEFAULT 0,
  `money` bigint NOT NULL DEFAULT 0,
  `score` int NOT NULL DEFAULT 0,
  `skin` smallint NOT NULL DEFAULT 0,
  `xp` int NOT NULL DEFAULT 0,
  `level` smallint NOT NULL DEFAULT 1,
  PRIMARY KEY (`u_id`) USING BTREE,
  CONSTRAINT `player_stats_ibfk_1` FOREIGN KEY (`u_id`) REFERENCES `players` (`u_id`) ON DELETE CASCADE ON UPDATE RESTRICT
) ENGINE = InnoDB AUTO_INCREMENT = 5 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of player_stats
-- ----------------------------
INSERT INTO `player_stats` VALUES (1, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1);
INSERT INTO `player_stats` VALUES (2, 0, 76, 0, 2, 0, 0, 0, 0, 4520, 0, 2, 0, 1);
INSERT INTO `player_stats` VALUES (3, 100, 100, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0);
INSERT INTO `player_stats` VALUES (4, 100, 100, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1);

-- ----------------------------
-- Table structure for player_vehicles
-- ----------------------------
DROP TABLE IF EXISTS `player_vehicles`;
CREATE TABLE `player_vehicles`  (
  `vehicle_id` int NOT NULL AUTO_INCREMENT,
  `u_id` int NULL DEFAULT NULL,
  `model_id` smallint NOT NULL,
  `x` float NULL DEFAULT NULL,
  `y` float NOT NULL,
  `z` float NOT NULL,
  `rot` float NOT NULL,
  `color_1` tinyint NOT NULL,
  `color_2` tinyint NOT NULL,
  `intid` tinyint NOT NULL,
  `vworld` smallint NOT NULL,
  `plate` varchar(8) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL DEFAULT '' COMMENT 'Bien so xe IC',
  PRIMARY KEY (`vehicle_id`) USING BTREE,
  INDEX `u_id`(`u_id` ASC) USING BTREE,
  CONSTRAINT `player_vehicles_ibfk_1` FOREIGN KEY (`u_id`) REFERENCES `players` (`u_id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of player_vehicles
-- ----------------------------

-- ----------------------------
-- Table structure for player_weapons
-- ----------------------------
DROP TABLE IF EXISTS `player_weapons`;
CREATE TABLE `player_weapons`  (
  `u_id` int NOT NULL AUTO_INCREMENT,
  `weapid` int NOT NULL,
  `ammo` int NOT NULL,
  UNIQUE INDEX `u_id`(`u_id` ASC, `weapid` ASC) USING BTREE,
  CONSTRAINT `player_weapons_ibfk_1` FOREIGN KEY (`u_id`) REFERENCES `players` (`u_id`) ON DELETE CASCADE ON UPDATE RESTRICT
) ENGINE = InnoDB AUTO_INCREMENT = 2 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of player_weapons
-- ----------------------------
INSERT INTO `player_weapons` VALUES (1, 22, 198);
INSERT INTO `player_weapons` VALUES (1, 25, 199);

-- ----------------------------
-- Table structure for players
-- ----------------------------
DROP TABLE IF EXISTS `players`;
CREATE TABLE `players`  (
  `u_id` int NOT NULL AUTO_INCREMENT,
  `account_id` int NULL DEFAULT NULL,
  `is_character` tinyint(1) NOT NULL DEFAULT 0,
  `username` varchar(24) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL,
  `password` char(60) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL,
  `firstname` varchar(24) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL DEFAULT '',
  `lastname` varchar(24) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL DEFAULT '',
  `age` tinyint NOT NULL DEFAULT 18,
  `gender` tinyint(1) NOT NULL DEFAULT 0,
  `origin` varchar(32) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL DEFAULT '',
  `background` varchar(96) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL DEFAULT '',
  `register_date` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `last_login` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `email` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL DEFAULT '',
  `activated` tinyint(1) NOT NULL DEFAULT 0,
  `otp_code` varchar(6) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL,
  PRIMARY KEY (`u_id`) USING BTREE,
  UNIQUE INDEX `username`(`username` ASC) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 5 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of players
-- ----------------------------
INSERT INTO `players` VALUES (1, NULL, 0, 'Khoi_Nguyen', '$2b$12$KLsQEEyqW3zeqjPbVXe6Qu6YvVxoa8s2GCqCLIJE8uuu23pMPgLPa', '', '', 18, 0, '', '', '2026-07-27 16:08:01', '2026-07-29 03:06:09', '', 0, NULL);
INSERT INTO `players` VALUES (2, 1, 1, 'JeyBee_Nguyen', 'CHARACTER_SLOT', 'JeyBee', 'Nguyen', 18, 0, 'Los Santos', 'Chua cap nhat', '2026-07-27 23:04:35', '2026-07-29 03:06:09', '', 0, NULL);
INSERT INTO `players` VALUES (3, 1, 1, 'Bii', 'CHARACTER_SLOT', 'Bii', '', 18, 0, 'Los Santos', 'Chua cap nhat', '2026-07-27 23:15:37', '2026-07-27 23:46:38', '', 0, NULL);
INSERT INTO `players` VALUES (4, NULL, 0, 'Khoi_Nguyenz', '$2b$12$NzSxkKNJYYT4G3QyTJWir.ExNS2fwEtC73ICXYEqwuseFxigyAfd.', '', '', 18, 0, '', '', '2026-07-29 19:12:17', '2026-07-29 19:12:17', '', 0, NULL);

-- ----------------------------
-- Table structure for turfs
-- ----------------------------
DROP TABLE IF EXISTS `turfs`;
CREATE TABLE `turfs`  (
  `turf_id` int NOT NULL AUTO_INCREMENT,
  `name` varchar(64) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL,
  `x` float NOT NULL,
  `y` float NOT NULL,
  `z` float NOT NULL,
  `radius` float NOT NULL DEFAULT 80,
  `gang_id` int NULL DEFAULT NULL COMMENT 'NULL = khong co chu',
  `color` int NOT NULL DEFAULT 0 COMMENT 'Mau hien thi tren map',
  `last_capture` datetime NULL DEFAULT NULL,
  PRIMARY KEY (`turf_id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 11 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of turfs
-- ----------------------------
INSERT INTO `turfs` VALUES (1, 'Ganton', 2435, -1698, 13.5, 100, NULL, 0, NULL);
INSERT INTO `turfs` VALUES (2, 'Glen Park', 1795, -1584, 13.4, 90, NULL, 0, NULL);
INSERT INTO `turfs` VALUES (3, 'Idlewood', 1870, -1869, 13.2, 100, NULL, 0, NULL);
INSERT INTO `turfs` VALUES (4, 'East Beach', 2593, -1508, 14, 90, NULL, 0, NULL);
INSERT INTO `turfs` VALUES (5, 'Jefferson', 2096, -1770, 13.3, 80, NULL, 0, NULL);
INSERT INTO `turfs` VALUES (6, 'Willowfield', 2376, -1928, 13.1, 80, NULL, 0, NULL);
INSERT INTO `turfs` VALUES (7, 'Playa Del Seville', 2636, -1848, 13.6, 80, NULL, 0, NULL);
INSERT INTO `turfs` VALUES (8, 'El Corona', 1970, -2112, 13.5, 90, NULL, 0, NULL);
INSERT INTO `turfs` VALUES (9, 'Little Mexico', 1703, -2082, 13.5, 80, NULL, 0, NULL);
INSERT INTO `turfs` VALUES (10, 'Chamberlain Hills', 2118, -1835, 13.4, 85, NULL, 0, NULL);

-- ----------------------------
-- Table structure for vehicle_components
-- ----------------------------
DROP TABLE IF EXISTS `vehicle_components`;
CREATE TABLE `vehicle_components`  (
  `vehicle_id` int NOT NULL,
  `component_id` smallint NOT NULL,
  `slot` smallint NOT NULL,
  PRIMARY KEY (`vehicle_id`, `slot`) USING BTREE,
  CONSTRAINT `vehicle_components_ibfk_1` FOREIGN KEY (`vehicle_id`) REFERENCES `player_vehicles` (`vehicle_id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of vehicle_components
-- ----------------------------

-- ----------------------------
-- Table structure for vehicle_trunk
-- ----------------------------
DROP TABLE IF EXISTS `vehicle_trunk`;
CREATE TABLE `vehicle_trunk`  (
  `trunk_id` int NOT NULL AUTO_INCREMENT,
  `vehicle_id` int NOT NULL COMMENT 'player_vehicles.vehicle_id',
  `item_name` varchar(64) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL,
  `item_label` varchar(64) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL DEFAULT '',
  `quantity` int NOT NULL DEFAULT 1,
  `weight` float NOT NULL DEFAULT 0,
  PRIMARY KEY (`trunk_id`) USING BTREE,
  INDEX `idx_vehicleid`(`vehicle_id` ASC) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of vehicle_trunk
-- ----------------------------

-- ----------------------------
-- Table structure for vips
-- ----------------------------
DROP TABLE IF EXISTS `vips`;
CREATE TABLE `vips`  (
  `u_id` int NOT NULL AUTO_INCREMENT,
  `vip_level` tinyint NOT NULL DEFAULT 0,
  `vip_expire_date` datetime NULL DEFAULT NULL,
  PRIMARY KEY (`u_id`) USING BTREE,
  CONSTRAINT `vips_ibfk_1` FOREIGN KEY (`u_id`) REFERENCES `players` (`u_id`) ON DELETE CASCADE ON UPDATE RESTRICT
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of vips
-- ----------------------------

SET FOREIGN_KEY_CHECKS = 1;
