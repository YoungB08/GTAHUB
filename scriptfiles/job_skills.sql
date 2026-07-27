-- scriptfiles/job_skills.sql
-- Bảng lưu Level/XP Nghề nghiệp theo từng Job cho mỗi nhân vật

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
