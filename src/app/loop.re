open Types;

module type Loopz = {let isPlayerActive: Types.player => bool; let incrementPositions: Types.area => Types.area;};

module Loop: Loopz = {
  let inc = stats => {...stats, position: stats.position +. stats.speed};
  let isPlayerActive = (player: player) => player.stats.position >= 1.;
  let incrementPositions =
    List.map((ys: list(place)) =>
      ys
      |> List.map((place: place) =>
           switch (place.state) {
           | EMPTY => place
           | PLAYER(p) => {...place, state: PLAYER({...p, stats: inc(p.stats)})}
           | ENEMY(e) => {...place, state: ENEMY({...e, stats: inc(e.stats)})}
           }
         )
    );
};
