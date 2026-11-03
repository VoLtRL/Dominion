# Dominion

## Technologies

Qt

## Points importants

- Interface graphique faite avec Qt en utilisant à la fois Qt designer et la librairie C++
- Connexion Client-Serveur faite avec QtNetwork, permettant de faire des requêtes TCP entre les acteurs.
- Gestion sécurisée de l'envoi des requêtes pour empêcher la triche (accéder aux cartes d'un autre, modifier son jeu illégalement)
- Interface adaptatif ne regénérant que ce qui est nécessaire à chaque étape pour éviter les lags.
- Intégration de Bots tournant sur le serveur en asynchrone, qui possèdent une stratégie parmi 3 (Big Money, Big Money terminal et Gardens Rush) et une difficulté permettant de régler le taux d'erreur d'application de la stratégie.
- classe Factory permettant de n'avoir qu'une instance de chaque carte stockée dans un `unique_ptr`.
- Polymorphisme des cartes pour spécifier leur type.
- Et plein d'autres trucs...

## Compilation

Pour build :
```sh
mkdir build && cd build && cmake .. && make
```

Pour lancer le serveur quand on est dans le build
```sh
./DominionServer
```

Pour lancer le client, il faut avoir Qt d'installé sur son ordinateur ( Très rapide sur linux, je sais pas pour windaube )
```sh
./DominionClient
```

## Idées d'implémentation

- Mode de jeu en ligne

- Voir pour le fonctionnement des réactions et des attaques, pour l'instant, les sélections fonctionnent. Revoir également le placement des cardsInPlay, des mains et des piles de défausse.
- Faire en sorte que le shop ne se load plus à chaque fois sans raison.
- Revoir les différents boutons et leur activation/désactivation selon le tour du joueur et la phase du jeu. Maybe les faire apparaître/disparaître selon le tour du joueur et la phase du jeu.

- Faut supprimer les différents qDebug et std::cout qui trainent dans le code, ça fait pas propre.
- Revoir les attaques, faut tout tester, regarder les selections aussi.
- Revoir les logs aussi, modifier où elles sont placées et comment elles sont affichées.
- Rajouter un message pour le résultat d'une sélection.
