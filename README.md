# WWW-SQL-Designer-Helper
Помощник для создания и просмотра бд на сайте [WWW SQL Designer](http://ondras.zarovi.cz/sql/demo/)

Скачать скомпилированную программу можно (здесь)[https://github.com/catinwarmhands/WWW-SQL-Designer-Helper/releases] (Раздел Downloads - helper.exe). Или можете скомпилировать файл `helper.cpp` самостоятельно.

##Фичи:
* Создание XML-файла из вашего sql-кода для создания таблиц, который можно использовать на сайте [WWW SQL Designer](http://ondras.zarovi.cz/sql/demo/) для просмотра базы данных в виде UML-графа. Результат будет записан в файл `output.xml`
* Генерация SQL- файла с прописаными первичными и вторичными ключами (ссылками). Результат будет записан в файл `output.sql`

##Правила:
* В входном файле должны быть только команды create table
* Команды create table должны быть записаны в следующем формате (обратите внимание на расположение скобок у create table и то, что каждое поле находится на отдельной строке, без комментариев):
```sql
create table tablename(
  pk_a integer,
  fk_b integer not null,
);
```
* Поле, начинающееся с `pk_` считается первичным ключем, а поле, начинающееся с `fk_` считается вторичным ключем, ссылающимся на таблицу, содержащую первичный ключ с таким же именем. Если программа не может найти в файле таблицу, на которую ссылкается этот ключ, то будет выведено сообщение об ошибке с предложением ввести название таблицы вручную
* Если в таблице уже прописаны действия (`on update cascade` и так далее) с ключами, они будут сброшены на стандартные `on update cascade on delete restrict`

##Использование сгенерированного XML файла на сайте [WWW SQL Designer](http://ondras.zarovi.cz/sql/demo/)
1. Справа вверху на сайте кнопка `Save/load`
2. Вставляем содержиое файла `output.xml` в поле для ввода
3. Сверху кнопка `Load XML`
