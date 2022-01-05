( define ( domain depots )
  ( :requirements :strips :typing :equality :htn )
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

  ( :method Make-Clear
    :parameters
    (
      ?it - surface
    )
    :precondition
    ( and
      ( clear ?it )
    )
    :subtasks
    (
    )
  )

  ( :method Make-Clear
    :parameters
    (
      ?it - surface
    )
    :vars
    (
      ?above_it - surface
      ?hoist - hoist
      ?place - place
    )
    :precondition
    ( and
      ( on ?above_it ?it )
      ( clear ?above_it )
      ( surface-at ?above_it ?place )
      ( hoist-at ?hoist ?place )
      ( available ?hoist )
    )
    :subtasks
    (
      ( !Lift ?hoist ?above_it ?it ?place )
    )
  )

  ( :method Make-Clear
    :parameters
    (
      ?it - surface
    )
    :vars
    (
      ?above_it - surface
      ?hoist - hoist
      ?other - surface
      ?place - place
      ?held - surface
    )
    :precondition
    ( and
      ( on ?above_it ?it )
      ( clear ?above_it )
      ( surface-at ?above_it ?place )
      ( surface-at ?other ?place )
      ( clear ?other )
      ( hoist-at ?hoist ?place )
      ( lifting ?hoist ?held )
      ( not ( = ?other ?above_it ) )
    )
    :subtasks
    (
      ( !Drop ?hoist ?held ?other ?place )
      ( Make-Clear ?it )
    )
  )

  ( :method Make-Clear
    :parameters
    (
      ?it - surface
    )
    :vars
    (
      ?above_it - surface
      ?hoist - hoist
      ?truck - truck
      ?place - place
      ?held - surface
    )
    :precondition
    ( and
      ( on ?above_it ?it )
      ( clear ?above_it )
      ( surface-at ?above_it ?place )
      ( hoist-at ?hoist ?place )
      ( truck-at ?truck ?place )
      ( lifting ?hoist ?held )
    )
    :subtasks
    (
      ( !Load ?hoist ?held ?truck ?place )
      ( Make-Clear ?it )
    )
  )

  ( :method Make-Clear
    :parameters
    (
      ?it - surface
    )
    :vars
    (
      ?above_it - surface
      ?hoist - hoist
      ?truck - truck
      ?place - place
      ?held - surface
      ?garage - place
    )
    :precondition
    ( and
      ( on ?above_it ?it )
      ( clear ?above_it )
      ( surface-at ?above_it ?place )
      ( hoist-at ?hoist ?place )
      ( truck-at ?truck ?garage )
      ( lifting ?hoist ?held )
      ( not ( = ?garage ?place ) )
    )
    :subtasks
    (
      ( !Drive ?truck ?garage ?place )
      ( Make-Clear ?it )
    )
  )

  ( :method Make-Clear
    :parameters
    (
      ?it - surface
    )
    :vars
    (
      ?above_it - surface
      ?further - surface
    )
    :precondition
    ( and
      ( on ?above_it ?it )
      ( on ?further ?above_it )
    )
    :subtasks
    (
      ( Make-Clear ?above_it )
      ( Make-Clear ?it )
    )
  )

  ( :method Make-Clear
    :parameters
    (
      ?it - surface
    )
    :vars
    (
      ?hoist - hoist
      ?other - surface
      ?place - place
    )
    :precondition
    ( and
      ( lifting ?hoist ?it )
      ( clear ?other )
      ( surface-at ?other ?place )
      ( hoist-at ?hoist ?place )
    )
    :subtasks
    (
      ( !Drop ?hoist ?it ?other ?place )
    )
  )

  ( :method Make-Clear
    :parameters
    (
      ?it - surface
    )
    :vars
    (
      ?hoist - hoist
      ?truck - truck
      ?place - place
    )
    :precondition
    ( and
      ( in ?it ?truck )
      ( truck-at ?truck ?place )
      ( hoist-at ?hoist ?place )
      ( available ?hoist )
    )
    :subtasks
    (
      ( !Unload ?hoist ?it ?truck ?place )
      ( Make-Clear ?it )
    )
  )

  ( :method Make-Clear
    :parameters
    (
      ?it - surface
    )
    :vars
    (
      ?hoist - hoist
      ?truck - truck
      ?place - place
      ?other - surface
      ?held - surface
    )
    :precondition
    ( and
      ( in ?it ?truck )
      ( truck-at ?truck ?place )
      ( hoist-at ?hoist ?place )
      ( lifting ?hoist ?held )
      ( clear ?other )
      ( surface-at ?other ?place )
    )
    :subtasks
    (
      ( !Drop ?hoist ?held ?other ?place )
      ( Make-Clear ?it )
    )
  )

  ( :method Make-Clear
    :parameters
    (
      ?it - surface
    )
    :vars
    (
      ?hoist - hoist
      ?truck - truck
      ?place - place
      ?held - surface
    )
    :precondition
    ( and
      ( in ?it ?truck )
      ( truck-at ?truck ?place )
      ( hoist-at ?hoist ?place )
      ( lifting ?hoist ?held )
    )
    :subtasks
    (
      ( !Load ?hoist ?it ?truck ?place )
      ( Make-Clear ?it )
    )
  )

  ( :method Make-Keep-Clear
    :parameters
    (
      ?it - surface
      ?leave - surface
    )
    :precondition
    ( and
      ( clear ?it )
      ( clear ?leave )
    )
    :subtasks
    (
    )
  )

  ( :method Make-Keep-Clear
    :parameters
    (
      ?it - surface
      ?leave - surface
    )
    :vars
    (
      ?above_it - surface
      ?hoist - hoist
      ?place - place
    )
    :precondition
    ( and
      ( clear ?leave )
      ( on ?above_it ?it )
      ( clear ?above_it )
      ( surface-at ?above_it ?place )
      ( hoist-at ?hoist ?place )
      ( available ?hoist )
    )
    :subtasks
    (
      ( !Lift ?hoist ?above_it ?it ?place )
    )
  )

  ( :method Make-Keep-Clear
    :parameters
    (
      ?it - surface
      ?leave - surface
    )
    :vars
    (
      ?above_it - surface
      ?hoist - hoist
      ?other - surface
      ?place - place
      ?held - surface
    )
    :precondition
    ( and
      ( clear ?leave )
      ( on ?above_it ?it )
      ( clear ?above_it )
      ( surface-at ?above_it ?place )
      ( surface-at ?other ?place )
      ( clear ?other )
      ( hoist-at ?hoist ?place )
      ( lifting ?hoist ?held )
      ( not ( = ?other ?above_it ) )
      ( not ( = ?other ?leave ) )
    )
    :subtasks
    (
      ( !Drop ?hoist ?held ?other ?place )
      ( Make-Keep-Clear ?it ?leave )
    )
  )

  ( :method Make-Keep-Clear
    :parameters
    (
      ?it - surface
      ?leave - surface
    )
    :vars
    (
      ?above_it - surface
      ?hoist - hoist
      ?truck - truck
      ?place - place
      ?held - surface
    )
    :precondition
    ( and
      ( clear ?leave )
      ( on ?above_it ?it )
      ( clear ?above_it )
      ( surface-at ?above_it ?place )
      ( hoist-at ?hoist ?place )
      ( truck-at ?truck ?place )
      ( lifting ?hoist ?held )
      ( not ( = ?leave ?held ) )
    )
    :subtasks
    (
      ( !Load ?hoist ?held ?truck ?place )
      ( Make-Keep-Clear ?it ?leave )
    )
  )

  ( :method Make-Keep-Clear
    :parameters
    (
      ?it - surface
      ?leave - surface
    )
    :vars
    (
      ?above_it - surface
      ?hoist - hoist
      ?truck - truck
      ?place - place
      ?other_place - place
      ?held - surface
    )
    :precondition
    ( and
      ( clear ?leave )
      ( on ?above_it ?it )
      ( clear ?above_it )
      ( surface-at ?above_it ?place )
      ( hoist-at ?hoist ?place )
      ( truck-at ?truck ?other_place )
      ( lifting ?hoist ?held )
      ( not ( = ?leave ?held ) )
      ( not ( = ?place ?other_place ) )
    )
    :subtasks
    (
      ( !Drive ?truck ?other_place ?place )
      ( Make-Keep-Clear ?it ?leave )
    )
  )

  ( :method Make-Keep-Clear
    :parameters
    (
      ?it - surface
      ?leave - surface
    )
    :vars
    (
      ?above_it - surface
      ?further - surface
    )
    :precondition
    ( and
      ( clear ?leave )
      ( on ?above_it ?it )
      ( on ?further ?above_it )
    )
    :subtasks
    (
      ( Make-Keep-Clear ?above_it ?leave )
      ( Make-Keep-Clear ?it ?leave )
    )
  )

  ( :method Make-Keep-Clear
    :parameters
    (
      ?it - surface
      ?leave - surface
    )
    :vars
    (
      ?hoist - hoist
      ?other - surface
      ?place - place
    )
    :precondition
    ( and
      ( clear ?leave )
      ( lifting ?hoist ?it )
      ( clear ?other )
      ( surface-at ?other ?place )
      ( hoist-at ?hoist ?place )
      ( not ( = ?leave ?other ) )
    )
    :subtasks
    (
      ( !Drop ?hoist ?it ?other ?place )
    )
  )

  ( :method Make-Keep-Clear
    :parameters
    (
      ?it - surface
      ?leave - surface
    )
    :vars
    (
      ?hoist - hoist
      ?truck - truck
      ?place - place
    )
    :precondition
    ( and
      ( clear ?leave )
      ( lifting ?hoist ?it )
      ( truck-at ?truck ?place )
      ( hoist-at ?hoist ?place )
    )
    :subtasks
    (
      ( !Load ?hoist ?it ?truck ?place )
      ( Make-Keep-Clear ?it ?leave )
    )
  )

  ( :method Make-Keep-Clear
    :parameters
    (
      ?it - surface
      ?leave - surface
    )
    :vars
    (
      ?hoist - hoist
      ?truck - truck
      ?new - place
      ?old - place
    )
    :precondition
    ( and
      ( clear ?leave )
      ( lifting ?hoist ?it )
      ( hoist-at ?hoist ?new )
      ( truck-at ?truck ?old )
      ( not ( = ?old ?new ) )
    )
    :subtasks
    (
      ( !Drive ?truck ?old ?new )
      ( Make-Keep-Clear ?it ?leave )
    )
  )

  ( :method Make-Keep-Clear
    :parameters
    (
      ?it - surface
      ?leave - surface
    )
    :vars
    (
      ?hoist - hoist
      ?truck - truck
      ?place - place
    )
    :precondition
    ( and
      ( clear ?leave )
      ( in ?it ?truck )
      ( truck-at ?truck ?place )
      ( hoist-at ?hoist ?place )
      ( available ?hoist )
    )
    :subtasks
    (
      ( !Unload ?hoist ?it ?truck ?place )
      ( Make-Keep-Clear ?it ?leave )
    )
  )

  ( :method Make-Keep-Clear
    :parameters
    (
      ?it - surface
      ?leave - surface
    )
    :vars
    (
      ?hoist - hoist
      ?truck - truck
      ?place - place
      ?other - surface
      ?held - surface
    )
    :precondition
    ( and
      ( clear ?leave )
      ( in ?it ?truck )
      ( truck-at ?truck ?place )
      ( hoist-at ?hoist ?place )
      ( lifting ?hoist ?held )
      ( clear ?other )
      ( surface-at ?other ?place )
      ( not ( = ?other ?leave ) )
    )
    :subtasks
    (
      ( !Drop ?hoist ?held ?other ?place )
      ( Make-Keep-Clear ?it ?leave )
    )
  )

  ( :method Make-Keep-Clear
    :parameters
    (
      ?it - surface
      ?leave - surface
    )
    :vars
    (
      ?hoist - hoist
      ?truck - truck
      ?place - place
      ?held - surface
    )
    :precondition
    ( and
      ( clear ?leave )
      ( in ?it ?truck )
      ( truck-at ?truck ?place )
      ( hoist-at ?hoist ?place )
      ( lifting ?hoist ?held )
    )
    :subtasks
    (
      ( !Load ?hoist ?held ?truck ?place )
      ( Make-Keep-Clear ?it ?leave )
    )
  )

  ( :method Make-On
    :parameters
    (
      ?ab - surface
      ?be - surface
    )
    :precondition
    ( and
      ( on ?ab ?be )
    )
    :subtasks
    (
    )
  )

  ( :method Make-On
    :parameters
    (
      ?ab - surface
      ?be - surface
    )
    :vars
    (
      ?hoist - hoist
      ?place - place
    )
    :precondition
    ( and
      ( clear ?be )
      ( surface-at ?be ?place )
      ( lifting ?hoist ?ab )
      ( hoist-at ?hoist ?place )
    )
    :subtasks
    (
      ( !Drop ?hoist ?ab ?be ?place )
    )
  )

  ( :method Make-On
    :parameters
    (
      ?ab - surface
      ?be - surface
    )
    :vars
    (
      ?hoist - hoist
      ?place - place
      ?other - surface
    )
    :precondition
    ( and
      ( clear ?be )
      ( surface-at ?be ?place )
      ( clear ?ab )
      ( on ?ab ?other )
      ( surface-at ?ab ?place )
      ( hoist-at ?hoist ?place )
      ( available ?hoist )
    )
    :subtasks
    (
      ( !Lift ?hoist ?ab ?other ?place )
      ( Make-On ?ab ?be )
    )
  )

  ( :method Make-On
    :parameters
    (
      ?ab - surface
      ?be - surface
    )
    :vars
    (
      ?hoist - hoist
      ?place - place
      ?held - surface
      ?other - surface
    )
    :precondition
    ( and
      ( clear ?be )
      ( surface-at ?be ?place )
      ( clear ?ab )
      ( surface-at ?ab ?place )
      ( hoist-at ?hoist ?place )
      ( lifting ?hoist ?held )
      ( clear ?other )
      ( surface-at ?other ?place )
      ( not ( = ?other ?ab ) )
      ( not ( = ?other ?be ) )
    )
    :subtasks
    (
      ( !Drop ?hoist ?held ?other ?place )
      ( Make-On ?ab ?be )
    )
  )

  ( :method Make-On
    :parameters
    (
      ?ab - surface
      ?be - surface
    )
    :vars
    (
      ?hoist - hoist
      ?place - place
      ?held - surface
      ?truck - truck
    )
    :precondition
    ( and
      ( clear ?be )
      ( surface-at ?be ?place )
      ( clear ?ab )
      ( surface-at ?ab ?place )
      ( hoist-at ?hoist ?place )
      ( lifting ?hoist ?held )
      ( truck-at ?truck ?place )
    )
    :subtasks
    (
      ( !Load ?hoist ?held ?truck ?place )
      ( Make-On ?ab ?be )
    )
  )

  ( :method Make-On
    :parameters
    (
      ?ab - surface
      ?be - surface
    )
    :vars
    (
      ?hoist - hoist
      ?new - place
      ?held - surface
      ?truck - truck
      ?old - place
    )
    :precondition
    ( and
      ( clear ?be )
      ( surface-at ?be ?new )
      ( clear ?ab )
      ( surface-at ?ab ?new )
      ( hoist-at ?hoist ?new )
      ( lifting ?hoist ?held )
      ( truck-at ?truck ?old )
      ( not ( = ?old ?new ) )
    )
    :subtasks
    (
      ( !Drive ?truck ?old ?new )
      ( Make-On ?ab ?be )
    )
  )

  ( :method Make-On
    :parameters
    (
      ?ab - surface
      ?be - surface
    )
    :vars
    (
      ?hoist - hoist
      ?place - place
      ?truck - truck
    )
    :precondition
    ( and
      ( clear ?be )
      ( surface-at ?be ?place )
      ( hoist-at ?hoist ?place )
      ( truck-at ?truck ?place )
      ( in ?ab ?truck )
      ( available ?hoist )
    )
    :subtasks
    (
      ( !Unload ?hoist ?ab ?truck ?place )
      ( Make-On ?ab ?be )
    )
  )

  ( :method Make-On
    :parameters
    (
      ?ab - surface
      ?be - surface
    )
    :vars
    (
      ?hoist - hoist
      ?truck - truck
      ?place - place
      ?other - surface
      ?held - surface
    )
    :precondition
    ( and
      ( clear ?be )
      ( surface-at ?be ?place )
      ( hoist-at ?hoist ?place )
      ( truck-at ?truck ?place )
      ( in ?ab ?truck )
      ( lifting ?hoist ?held )
      ( clear ?other )
      ( surface-at ?other ?place )
      ( not ( = ?other ?be ) )
    )
    :subtasks
    (
      ( !Drop ?hoist ?held ?other ?place )
      ( Make-On ?ab ?be )
    )
  )

  ( :method Make-On
    :parameters
    (
      ?ab - surface
      ?be - surface
    )
    :vars
    (
      ?hoist - hoist
      ?truck - truck
      ?place - place
      ?held - surface
    )
    :precondition
    ( and
      ( clear ?be )
      ( surface-at ?be ?place )
      ( truck-at ?truck ?place )
      ( in ?ab ?truck )
      ( hoist-at ?hoist ?place )
      ( lifting ?hoist ?held )
    )
    :subtasks
    (
      ( !Load ?hoist ?held ?truck ?place )
      ( Make-On ?ab ?be )
    )
  )

  ( :method Make-On
    :parameters
    (
      ?ab - surface
      ?be - surface
    )
    :vars
    (
      ?truck - truck
      ?new - place
      ?old - place
    )
    :precondition
    ( and
      ( clear ?be )
      ( surface-at ?be ?new )
      ( in ?ab ?truck )
      ( truck-at ?truck ?old )
      ( not ( = ?old ?new ) )
    )
    :subtasks
    (
      ( !Drive ?truck ?old ?new )
      ( Make-On ?ab ?be )
    )
  )

  ( :method Make-On
    :parameters
    (
      ?ab - surface
      ?be - surface
    )
    :vars
    (
      ?truck - truck
      ?new - place
      ?old - place
      ?hoist - hoist
    )
    :precondition
    ( and
      ( clear ?be )
      ( surface-at ?be ?new )
      ( lifting ?hoist ?ab )
      ( hoist-at ?hoist ?old )
      ( truck-at ?truck ?old )
      ( not ( = ?old ?new ) )
    )
    :subtasks
    (
      ( !Load ?hoist ?ab ?truck ?old )
      ( Make-On ?ab ?be )
    )
  )

  ( :method Make-On
    :parameters
    (
      ?ab - surface
      ?be - surface
    )
    :vars
    (
      ?truck - truck
      ?new - place
      ?old - place
      ?hoist - hoist
      ?other - surface
    )
    :precondition
    ( and
      ( clear ?be )
      ( surface-at ?be ?new )
      ( truck-at ?truck ?old )
      ( not ( = ?old ?new ) )
      ( hoist-at ?hoist ?old )
      ( clear ?ab )
      ( surface-at ?ab ?old )
      ( available ?hoist )
      ( on ?ab ?other )
    )
    :subtasks
    (
      ( !Lift ?hoist ?ab ?other ?old )
      ( Make-On ?ab ?be )
    )
  )

  ( :method Make-On
    :parameters
    (
      ?ab - surface
      ?be - surface
    )
    :vars
    (
      ?truck - truck
      ?new - place
      ?old - place
      ?hoist - hoist
      ?other - surface
      ?held - surface
    )
    :precondition
    ( and
      ( clear ?be )
      ( surface-at ?be ?new )
      ( clear ?ab )
      ( surface-at ?ab ?old )
      ( not ( = ?old ?new ) )
      ( truck-at ?truck ?old )
      ( hoist-at ?hoist ?old )
      ( lifting ?hoist ?held )
      ( clear ?other )
      ( surface-at ?other ?old )
      ( not ( = ?other ?ab ) )
    )
    :subtasks
    (
      ( !Drop ?hoist ?held ?other ?old )
      ( Make-On ?ab ?be )
    )
  )

  ( :method Make-On
    :parameters
    (
      ?ab - surface
      ?be - surface
    )
    :vars
    (
      ?truck - truck
      ?old - place
      ?new - place
      ?hoist - hoist
      ?held - surface
    )
    :precondition
    ( and
      ( clear ?be )
      ( surface-at ?be ?new )
      ( clear ?ab )
      ( surface-at ?ab ?old )
      ( not ( = ?old ?new ) )
      ( truck-at ?truck ?old )
      ( hoist-at ?hoist ?old )
      ( lifting ?hoist ?held )
    )
    :subtasks
    (
      ( !Load ?hoist ?held ?truck ?old )
      ( Make-On ?ab ?be )
    )
  )

  ( :method Make-On
    :parameters
    (
      ?ab - surface
      ?be - surface
    )
    :vars
    (
      ?truck - truck
      ?new - place
      ?old - place
      ?garage - place
    )
    :precondition
    ( and
      ( clear ?be )
      ( surface-at ?be ?new )
      ( clear ?ab )
      ( surface-at ?ab ?old )
      ( truck-at ?truck ?garage )
      ( not ( = ?garage ?old ) )
    )
    :subtasks
    (
      ( !Drive ?truck ?garage ?old )
      ( Make-On ?ab ?be )
    )
  )
      
  ( :method Make-On
    :parameters
    (
      ?ab - surface
      ?be - surface
    )
    :vars
    (
      ?other - surface
    )
    :precondition
    ( and
      ( clear ?be )
      ( on ?other ?ab )
    )
    :subtasks
    (
      ( Make-Keep-Clear ?ab ?be )
      ( Make-On ?ab ?be )
    )
  )

  ( :method Make-On
    :parameters
    (
      ?ab - surface
      ?be - surface
    )
    :vars
    (
      ?other - surface
    )
    :precondition
    ( and
      ( on ?other ?be )
    )
    :subtasks
    (
      ( Make-Clear ?be )
      ( Make-On ?ab ?be )
    )
  )
      
)
