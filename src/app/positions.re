open Types;

module BasicPositions: Positions = {
  let divisor = 2.;
  let isActive = stats => stats.position >= 1.;
  let increment = stats => {...stats, position: stats.position +. (stats.speed /. divisor)};
  let incrementAll =
    List.map((ys: list(place)) =>
      ys
      |> List.map((place: place) =>
           switch (place.state) {
           | EMPTY => place
           | PLAYER(p) => {...place, state: PLAYER({...p, stats: increment(p.stats)})}
           | ENEMY(e) => {...place, state: ENEMY({...e, stats: increment(e.stats)})}
           }
         )
    );
};
