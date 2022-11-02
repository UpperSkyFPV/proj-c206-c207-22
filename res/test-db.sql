USE `maindb`;

INSERT INTO `Address`(`host`, `port`) VALUES
            ('Localhost', 3210),
            ('0.0.0.0', 3210);

INSERT INTO `User`(`name`, `user_address`) VALUES
            ('Bob', 1),
            ('John', 2);
            
INSERT INTO `Chat`(`name`, `description`) VALUES
            ('The Chat', 'This is the incredable chat!');

INSERT INTO `Chat_has_User`(`Chat_id`, `User_id`) VALUES
            (1, 1),
            (1, 2);
            
INSERT INTO `Message`(`content`, `in_chat`, `sent_by`) VALUES
            ('Hey there!', 1, 1),
            ('How are you?', 1, 1),
            ('Fine, thanks!', 1, 2);
            
SELECT * FROM `Message`;
            
SELECT A.`host`, A.`port` FROM `Chat_has_User` AS CU
                                       JOIN `Chat`    AS C ON C.`id` = CU.`Chat_id`
                                       JOIN `User`    AS U ON U.`id` = CU.`User_id`
                                       JOIN `Address` AS A ON A.`id` = U.`user_address`
                                       WHERE U.`id` != 2;

UPDATE `Message` SET `sent` = 1 WHERE id = 1;










