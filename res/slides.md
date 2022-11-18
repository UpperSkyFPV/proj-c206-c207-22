# TermGram - Terminal messaging

Um sistema de mensagens extremamente básico para se comunicar sem sair do terminal.

----

> Lucas Ross - GEC 1718

---

# Tecs

- C++:
    1. Linguagem escolhida por permitir código em alto nível ao mesmo tempo que se mantém performance.
    2. Pode ser compilada para facilitar a distribuição.
    3. Não precisa de um runtime instalado.
- SQLite:
    1. Banco de dados escolhido por não precisar de um servidor separado rodando.
    2. Compativel quase perfeitamente com MySQL.
    3. Um único arquivo gerado no sistema de arquivos.
- Comunicação UDP:
    1. Porque não?
- MessagePack:
    1. Confiável e compacto.

---

# Banco de dados

O banco de dados possui 5 tabelas bem simples:

```
             ┌──────────┐
    ┌─────── │ Message  │
    │        └──────────┘
    │
    │           │
    │           │
    │           │
    ▼           ▼
┌──────┐     ┌──────┐                    ┌─────────┐
│ Chat │     │ User │ ─────────────────► │ Address │
└──────┘     └──────┘                    └─────────┘
    ▲           ▲
    │           │
    │           │
    │           │
    │
    │        ┌───────────────┐
    └─────── │ Chat_has_User │
             └───────────────┘
```

---

## User

Armazena cada um dos usuários, que possui somente um nome e um endereço.
Está em uma relação `1:1` com a tabela _Address_ e em uma relação `N:N` com a
tabela _Chat_.

```sql
CREATE TABLE `User` (
    `id`           INTEGER PRIMARY KEY,
    `name`         VARCHAR(45) NOT NULL,
    `user_address` INT         NOT NULL,
    CONSTRAINT `fk_User_Address`
        FOREIGN KEY (`user_address`)
        REFERENCES `Address` (`id`)
        ON DELETE NO ACTION
        ON UPDATE NO ACTION
);
```

---

## Address

Armazena os endereços de transmissão para cada usuário, que é constituído
somente de um endereço e porta. Está em uma relação `1:1` com a tabela _User_.

```sql
CREATE TABLE `Address` (
    `id`   INTEGER PRIMARY KEY,
    `host` VARCHAR(45) NOT NULL,
    `port` INT         NOT NULL
);
```

---

## Chat

Armazena todos os chats. Cada chat possui um nome e uma breve descrição.
Está em uma relação `N:N` com a tabela _User_.

```sql
CREATE TABLE `Chat` (
    `id`          INTEGER PRIMARY KEY,
    `name`        VARCHAR(45) NOT NULL,
    `description` VARCHAR(45) NOT NULL
);
```

---

## Message

Armazena todas as mensagens do sistema. Cada mensagem possui um conteúdo e
vários parâmetros para indicar seu estado, mas nem todos acabaram não sendo
usados. Está em uma relação `1:N` com as tabelas _Chat_ e _User_.

```sql
CREATE TABLE `Message` (
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
```

---

## Chat_has_User

Usada para representar a relação `N:N` das tabelas _User_ e _Chat_.

```sql
CREATE TABLE `Chat_has_User` (
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
```

