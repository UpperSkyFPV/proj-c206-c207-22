# TermGram - Terminal messaging

Um sistema de mensagens _extremamente básico_ para se comunicar sem sair do terminal.

----

> Lucas Ross - GEC 1718
> Projeto de C206 e C207 - 2022/2

---

# Banco de dados

O banco de dados foi arquitetado de forma simples, com apenas 5 tabelas:

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

---

# Funcionamento

O procedimento para enviar uma mensagem é bastante simples:

- Um usuário escreve uma mensagem no campo de texto.
- O applicativo descobre no banco o endereço e nome de todos os contatos naquele chat.
- O applicativo envia para cada um dos endereços a mensagem codificada como [msgpack](https://msgpack.org/)

Formato de uma mensagem:
```json
{
    "content": "Hello!",
    "sent_by": "arnold",
    "sent_from": "terminator"
}
```

Onde:

- `content` é o conteúdo da mensagem em UTF-8.
- `sent_by`: é o nome do unsuário que enviou a mensagem.
- `sent_from`: é o nome do chat de onde a mensagem foi enviada.

---

# Funcionamento

O procedimento para receber uma mensagem é igualmente simples:

- O aplicativo está escutando na porta programada e recebe um pacote.
- O pacote é decodificado e então o conteúdo é inserido no chat que possui o nome recebido.

---

# GUI

A interface "gráfica" do app é bastate básica, evitando fazer mais que o minimamente
necessário, em uma tentativa de evitar o crescimento exponencial de complexidade que
um sistema de GUI responsivo criaria.

Por trás do sistema existem alguns pacotes bastante específicos para coordenar a app:

- `db`: Um módulo de abstração do banco de dados SQLite.
- `term`: Um módulo de abstração do terminal em um buffer de "pixels".
- `eng`: Um módulo de abstração do sistema de I/O e layout necessários para se criar uma interface apresentável.
- `app`: Um módulo com o código e lógica do app em si.
- `models`: Um módulo com diversas classes para organizar os dados armazenados nas tabelas do banco.
- `dao`: Um módulo de abstrações sobre o driver do SQLite para carregar e salvar `models` diretamente.
- `os`: Um módulo de utilidades para interagir com o sistema de arquivos do sistema operacional.


---

# Usabilidade

Na parte inferior da tela ficam ajudas para alguns comandos básicos, mas esses são:

- `<ctrl+q>`: Para sair do app a qualquer momento.
- `<ctrl+u>`: Para se criar um novo usuário. No popup se usa `<tab>` para se circular a seleção pelos items. Para se adentrar uma caixa de texto, basta pressionar `<enter>` e digitar (somente é possível sair confirmando o valor digitado com outro `<enter>`). Para se sair do popup basta usar `<esc>`.
- `<ctrl+c>`: Para se criar um novo chat. Os comandos internos são exatemente iguais ao de se criar um usuário.
- `<c>`: Para se circular pelos chats no menu esquerdo.
- `<m>`: Para se escrever uma mensagem no chat selecionado.
- `<ctrl+a>`: Para se adicionar um usuário a um chat. Usa-se `<tab>` para circular pelos usuário disponíveis para se adicionar ao chat e `<enter>` para confirmar. `<esc>` para sair.
- `<ctrl+d>`: Para se remover um usuário do chat selecionado (mesmos controles de se adicionar um usuário).

> Nota: Existem algumas complicações com adicionar e remover usuários, por isso é bom sair e entrar de novo no app (com `<ctrl+q>`).

## Configuração de usuário e porta

As variáveis de contexto `NAME` e `PORT` são usadas para se configurar qual é o nome do usário rodando o programa (o que é usado para resolução de remetente).

---

# Demo Time!

