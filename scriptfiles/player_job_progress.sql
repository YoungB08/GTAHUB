CREATE TABLE IF NOT EXISTS player_job_progress (
    u_id INT NOT NULL,
    job_id SMALLINT NOT NULL,
    job_xp INT NOT NULL DEFAULT 0,
    job_level SMALLINT NOT NULL DEFAULT 1,
    PRIMARY KEY (u_id, job_id),
    FOREIGN KEY (u_id) REFERENCES players(u_id) ON DELETE CASCADE ON UPDATE CASCADE
);
