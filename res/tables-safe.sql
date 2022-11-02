-- -----------------------------------------------------
-- Table `Chat`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `Chat` (
    `id`          INTEGER PRIMARY KEY,
    `name`        VARCHAR(45) NOT NULL,
    `description` VARCHAR(45) NOT NULL
);

-- -----------------------------------------------------
-- Table `Address`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `Address` (
    `id`   INTEGER PRIMARY KEY,
    `host` VARCHAR(45) NOT NULL,
    `port` INT         NOT NULL
);

-- -----------------------------------------------------
-- Table `User`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `User` (
    `id`           INTEGER PRIMARY KEY,
    `name`         VARCHAR(45) NOT NULL,
    `user_address` INT         NOT NULL,
    CONSTRAINT `fk_User_Address`
        FOREIGN KEY (`user_address`)
        REFERENCES `Address` (`id`)
        ON DELETE NO ACTION
        ON UPDATE NO ACTION
);

-- -----------------------------------------------------
-- Table `Chat_has_User`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `Chat_has_User` (
    `Chat_id` INT NOT NULL,
    `User_id` INT NOT NULL,
    PRIMARY KEY (`Chat_id`, `User_id`),
    CONSTRAINT `fk_Chat_has_User_Chat1`
        FOREIGN KEY (`Chat_id`)
        REFERENCES `Chat` (`id`)
        ON DELETE NO ACTION
        ON UPDATE NO ACTION,
    CONSTRAINT `fk_Chat_has_User_User1`
        FOREIGN KEY (`User_id`)
        REFERENCES `User` (`id`)
        ON DELETE NO ACTION
        ON UPDATE NO ACTION
);

-- -----------------------------------------------------
-- Table `Message`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `Message` (
    `id`       INTEGER PRIMARY KEY,
    `content`  VARCHAR(255) NOT NULL,
    `sent`     TINYINT NOT NULL DEFAULT 0,
    `received` TINYINT NOT NULL DEFAULT 0,
    `error`    VARCHAR(45) NULL,
    `in_chat`  INT NOT NULL,
    `sent_by`  INT NOT NULL,
    CONSTRAINT `fk_Message_Chat1`
        FOREIGN KEY (`in_chat`)
        REFERENCES `Chat` (`id`)
        ON DELETE NO ACTION
        ON UPDATE NO ACTION,
    CONSTRAINT `fk_Message_User1`
        FOREIGN KEY (`sent_by`)
        REFERENCES `User` (`id`)
        ON DELETE NO ACTION
        ON UPDATE NO ACTION
);
