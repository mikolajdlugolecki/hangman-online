# Wisielec przez sieć

### Opis:

Gracz łączy się do serwera i wysyła swój nick (jeśli nick jest już zajęty,
serwer prosi o podanie innego nicku).

Po wybraniu nicku gracz trafia do menu, w którym ma dwie opcje:

- utwórz pokój,

- dołącz do pokoju.

### Tworzenie pokoju:

Po wybraniu opcji utworzenia pokoju serwer:

- nadaje temu pokojowi kolejny wolny numer (np. 17).

- generuje losowy 4-cyfrowy PIN (np. 4821).

- zwraca graczowi numer pokoju oraz PIN.

Gracz trafia następnie do tego pokoju jako jego pierwszy uczestnik.
W pokoju widzi listę graczy (na początku tylko siebie) oraz może rozpocząć grę,
o ile w pokoju będzie przynajmniej dwóch graczy.

### Dołączanie do pokoju:

Gracz wybiera opcję „Dołącz do pokoju”, po czym wpisuje:

- numer pokoju,

- PIN-hasło.

Serwer sprawdza poprawność danych. Jeśli numer lub PIN jest niepoprawny, odsyła
komunikat o błędzie i prosi o ponowne podanie danych.

Poprawnie zweryfikowany gracz dołącza do pokoju i widzi listę wszystkich
uczestników.

### Tryb rywalizacyjny:

Po rozpoczęciu gry serwer losuje jedno wspólne hasło dla wszystkich graczy.
Każdy uczestnik otrzymuje osobny stan swojej planszy (odkryte litery, liczba
pozostałych prób, czas rundy).

Gracze niezależnie zgadują litery, a serwer prowadzi oddzielny stan gry dla
każdego z nich, lecz hasło jest wspólne, aby szanse były równe. 

Serwer na bieżąco informuje wszystkich graczy o
aktualnych statystykach uczestników (nick, punkty, błędy oraz status połączenia).

Gdy dany gracz:

- odgadnie całe hasło

lub

- wyczerpie próby,

kończy swoją rundę i otrzymuje wynik.

Gdy wszyscy gracze zakończą swoje rundy albo opuszczą pokój — gra się kończy.
Wszyscy widzą ranking na podstawie wyników.

### Utrata połączenia:

Jeśli gracz świadomie opuści pokój w trakcie gry, nie może wrócić do
trwającej rozgrywki.

Jeśli gracz straci połączenie, klient i serwer będą do skutku próbować ponownie je nawiązać.
Po ponownym połączeniu gracz kontynuuje grę ze swoim poprzednim stanem — zachowując odgadnięte litery oraz liczbę pozostałych prób.

Gracz nie odzyskuje utraconego czasu – czas rundy płynie dalej tak, jakby nadal był w grze. Oznacza to, że dłuższa nieobecność może doprowadzić do automatycznego przegrania rundy z powodu upływu czasu.

Jeśli gracz odzyska połączenie po tym, jak rozgrywka w pokoju już się zakończyła, serwer wysyła stosowną informację i przenosi gracza z powrotem do lobby.

Gracz z zerwanym połączeniem pozostaje widoczny na liście jako „rozłączony”.
