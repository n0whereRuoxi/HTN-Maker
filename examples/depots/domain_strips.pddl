( define ( domain depots )
  ( :requirements :strips :typing :equality )
  ( :types place truck hoist surface )

  ( :predicates
    ( surface-at ?a - surface ?b - place )
    ( on ?c - surface ?d - surface )
    ( in ?e - surface ?f - truck )
    ( lifting ?g - hoist ?h - surface )
    ( available ?i - hoist )
    ( clear ?j - surface )
    ( is-crate ?k - surface )
    ( hoist-at ?l - hoist ?m - place )
    ( truck-at ?n - truck ?o - place )
  )

  ( :action !Drive
    :parameters
    (
      ?truck - truck
      ?old - place
      ?new - place
    )
    :precondition
    ( and
      ( truck-at ?truck ?old )
      ( not ( = ?old ?new ) )
    )
    :effect
    ( and
      ( not ( truck-at ?truck ?old ) )
      ( truck-at ?truck ?new )
    )
  )

  ( :action !Lift
    :parameters
    (
      ?hoist - hoist
      ?crate - surface
      ?below - surface
      ?here - place
    )
    :precondition
    ( and
      ( hoist-at ?hoist ?here )
      ( available ?hoist )
      ( surface-at ?crate ?here )
      ( on ?crate ?below )
      ( clear ?crate )
      ( is-crate ?crate )
    )
    :effect
    ( and
      ( not ( surface-at ?crate ?here ) )
      ( not ( clear ?crate ) )
      ( not ( available ?hoist ) )
      ( not ( on ?crate ?below ) )
      ( clear ?below )
      ( lifting ?hoist ?crate )
    )
  )

  ( :action !Drop
    :parameters
    (
      ?hoist - hoist
      ?crate - surface
      ?below - surface
      ?here - place
    )
    :precondition
    ( and
      ( hoist-at ?hoist ?here )
      ( surface-at ?below ?here )
      ( clear ?below )
      ( lifting ?hoist ?crate )
      ( is-crate ?crate )
    )
    :effect
    ( and
      ( not ( lifting ?hoist ?crate ) )
      ( not ( clear ?below ) )
      ( available ?hoist )
      ( surface-at ?crate ?here )
      ( clear ?crate )
      ( on ?crate ?below )
    )
  )

  ( :action !Load
    :parameters
    (
      ?hoist - hoist
      ?crate - surface
      ?truck - truck
      ?here - place
    )
    :precondition
    ( and
      ( hoist-at ?hoist ?here )
      ( truck-at ?truck ?here )
      ( lifting ?hoist ?crate )
      ( is-crate ?crate )
    )
    :effect
    ( and
      ( not ( lifting ?hoist ?crate ) )
      ( in ?crate ?truck )
      ( available ?hoist )
    )
  )

  ( :action !Unload
    :parameters
    (
      ?hoist - hoist
      ?crate - surface
      ?truck - truck
      ?here - place
    )
    :precondition
    ( and
      ( hoist-at ?hoist ?here )
      ( truck-at ?truck ?here )
      ( available ?hoist )
      ( in ?crate ?truck )
      ( is-crate ?crate )
    )
    :effect
    ( and
      ( not ( in ?crate ?truck ) )
      ( not ( available ?hoist ) )
      ( lifting ?hoist ?crate )
    )
  )

)
