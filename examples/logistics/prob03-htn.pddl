( define ( htn-problem probname )

  ( :domain logistics )

  ( :requirements :strips :htn :typing :equality )

  ( :objects
    a000 - airplane
    a001 - airplane
    a002 - airplane
    a003 - airplane
    c000 - city
    c001 - city
    c002 - city
    c003 - city
    c004 - city
    c005 - city
    c006 - city
    c007 - city
    c008 - city
    t000-000 - truck
    t000-001 - truck
    t000-002 - truck
    t000-003 - truck
    t001-000 - truck
    t001-001 - truck
    t001-002 - truck
    t001-003 - truck
    t002-000 - truck
    t002-001 - truck
    t002-002 - truck
    t002-003 - truck
    t003-000 - truck
    t003-001 - truck
    t003-002 - truck
    t004-000 - truck
    t004-001 - truck
    t004-002 - truck
    t004-003 - truck
    t005-000 - truck
    t005-001 - truck
    t005-002 - truck
    t005-003 - truck
    t006-000 - truck
    t006-001 - truck
    t006-002 - truck
    t006-003 - truck
    t007-000 - truck
    t007-001 - truck
    t007-002 - truck
    t007-003 - truck
    t008-000 - truck
    t008-001 - truck
    t008-002 - truck
    l000-000 - location
    l000-001 - location
    l000-002 - location
    l000-003 - location
    l000-004 - location
    l000-005 - location
    l000-006 - location
    l000-007 - location
    l000-008 - location
    l000-009 - location
    l001-000 - location
    l001-001 - location
    l001-002 - location
    l001-003 - location
    l001-004 - location
    l001-005 - location
    l001-006 - location
    l001-007 - location
    l001-008 - location
    l001-009 - location
    l001-010 - location
    l002-000 - location
    l002-001 - location
    l002-002 - location
    l002-003 - location
    l002-004 - location
    l002-005 - location
    l002-006 - location
    l002-007 - location
    l002-008 - location
    l003-000 - location
    l003-001 - location
    l003-002 - location
    l003-003 - location
    l003-004 - location
    l003-005 - location
    l003-006 - location
    l003-007 - location
    l003-008 - location
    l003-009 - location
    l003-010 - location
    l004-000 - location
    l004-001 - location
    l004-002 - location
    l004-003 - location
    l004-004 - location
    l004-005 - location
    l004-006 - location
    l004-007 - location
    l005-000 - location
    l005-001 - location
    l005-002 - location
    l005-003 - location
    l005-004 - location
    l005-005 - location
    l005-006 - location
    l005-007 - location
    l006-000 - location
    l006-001 - location
    l006-002 - location
    l006-003 - location
    l006-004 - location
    l006-005 - location
    l006-006 - location
    l006-007 - location
    l006-008 - location
    l007-000 - location
    l007-001 - location
    l007-002 - location
    l007-003 - location
    l007-004 - location
    l007-005 - location
    l007-006 - location
    l007-007 - location
    l007-008 - location
    l007-009 - location
    l007-010 - location
    l008-000 - location
    l008-001 - location
    l008-002 - location
    l008-003 - location
    l008-004 - location
    l008-005 - location
    l008-006 - location
    l008-007 - location
    l008-008 - location
    l008-009 - location
    p000 - obj
    p001 - obj
    p002 - obj
    p003 - obj
    p004 - obj
    p005 - obj
    p006 - obj
    p007 - obj
    p008 - obj
    p009 - obj
    p010 - obj
    p011 - obj
    p012 - obj
    p013 - obj
    p014 - obj
    p015 - obj
    p016 - obj
    p017 - obj
    p018 - obj
    p019 - obj
    p020 - obj
    p021 - obj
    p022 - obj
    p023 - obj
    p024 - obj
    p025 - obj
    p026 - obj
    p027 - obj
    p028 - obj
    p029 - obj
    p030 - obj
    p031 - obj
    p032 - obj
    p033 - obj
    p034 - obj
    p035 - obj
    p036 - obj
    p037 - obj
    p038 - obj
    p039 - obj
    p040 - obj
    p041 - obj
    p042 - obj
    p043 - obj
    p044 - obj
    p045 - obj
    p046 - obj
    p047 - obj
    p048 - obj
    p049 - obj
    p050 - obj
    p051 - obj
    p052 - obj
    p053 - obj
    p054 - obj
    p055 - obj
    p056 - obj
    p057 - obj
    p058 - obj
    p059 - obj
    p060 - obj
    p061 - obj
    p062 - obj
    p063 - obj
    p064 - obj
    p065 - obj
    p066 - obj
    p067 - obj
    p068 - obj
    p069 - obj
    p070 - obj
    p071 - obj
    p072 - obj
    p073 - obj
    p074 - obj
    p075 - obj
    p076 - obj
    p077 - obj
    p078 - obj
    p079 - obj
  )

  ( :init
    ( in-city l000-000 c000 )
    ( in-city l000-001 c000 )
    ( in-city l000-002 c000 )
    ( in-city l000-003 c000 )
    ( in-city l000-004 c000 )
    ( in-city l000-005 c000 )
    ( in-city l000-006 c000 )
    ( in-city l000-007 c000 )
    ( in-city l000-008 c000 )
    ( in-city l000-009 c000 )
    ( in-city l001-000 c001 )
    ( in-city l001-001 c001 )
    ( in-city l001-002 c001 )
    ( in-city l001-003 c001 )
    ( in-city l001-004 c001 )
    ( in-city l001-005 c001 )
    ( in-city l001-006 c001 )
    ( in-city l001-007 c001 )
    ( in-city l001-008 c001 )
    ( in-city l001-009 c001 )
    ( in-city l001-010 c001 )
    ( in-city l002-000 c002 )
    ( in-city l002-001 c002 )
    ( in-city l002-002 c002 )
    ( in-city l002-003 c002 )
    ( in-city l002-004 c002 )
    ( in-city l002-005 c002 )
    ( in-city l002-006 c002 )
    ( in-city l002-007 c002 )
    ( in-city l002-008 c002 )
    ( in-city l003-000 c003 )
    ( in-city l003-001 c003 )
    ( in-city l003-002 c003 )
    ( in-city l003-003 c003 )
    ( in-city l003-004 c003 )
    ( in-city l003-005 c003 )
    ( in-city l003-006 c003 )
    ( in-city l003-007 c003 )
    ( in-city l003-008 c003 )
    ( in-city l003-009 c003 )
    ( in-city l003-010 c003 )
    ( in-city l004-000 c004 )
    ( in-city l004-001 c004 )
    ( in-city l004-002 c004 )
    ( in-city l004-003 c004 )
    ( in-city l004-004 c004 )
    ( in-city l004-005 c004 )
    ( in-city l004-006 c004 )
    ( in-city l004-007 c004 )
    ( in-city l005-000 c005 )
    ( in-city l005-001 c005 )
    ( in-city l005-002 c005 )
    ( in-city l005-003 c005 )
    ( in-city l005-004 c005 )
    ( in-city l005-005 c005 )
    ( in-city l005-006 c005 )
    ( in-city l005-007 c005 )
    ( in-city l006-000 c006 )
    ( in-city l006-001 c006 )
    ( in-city l006-002 c006 )
    ( in-city l006-003 c006 )
    ( in-city l006-004 c006 )
    ( in-city l006-005 c006 )
    ( in-city l006-006 c006 )
    ( in-city l006-007 c006 )
    ( in-city l006-008 c006 )
    ( in-city l007-000 c007 )
    ( in-city l007-001 c007 )
    ( in-city l007-002 c007 )
    ( in-city l007-003 c007 )
    ( in-city l007-004 c007 )
    ( in-city l007-005 c007 )
    ( in-city l007-006 c007 )
    ( in-city l007-007 c007 )
    ( in-city l007-008 c007 )
    ( in-city l007-009 c007 )
    ( in-city l007-010 c007 )
    ( in-city l008-000 c008 )
    ( in-city l008-001 c008 )
    ( in-city l008-002 c008 )
    ( in-city l008-003 c008 )
    ( in-city l008-004 c008 )
    ( in-city l008-005 c008 )
    ( in-city l008-006 c008 )
    ( in-city l008-007 c008 )
    ( in-city l008-008 c008 )
    ( in-city l008-009 c008 )
    ( airport l000-000 )
    ( airport l000-002 )
    ( airport l000-006 )
    ( airport l001-008 )
    ( airport l001-003 )
    ( airport l001-009 )
    ( airport l002-008 )
    ( airport l002-003 )
    ( airport l003-008 )
    ( airport l003-009 )
    ( airport l004-002 )
    ( airport l004-007 )
    ( airport l004-003 )
    ( airport l005-004 )
    ( airport l005-001 )
    ( airport l005-000 )
    ( airport l006-005 )
    ( airport l006-006 )
    ( airport l007-003 )
    ( airport l007-009 )
    ( airport l008-004 )
    ( airport l008-002 )
    ( airport l008-000 )
    ( truck-at t000-000 l000-005 )
    ( truck-at t000-001 l000-006 )
    ( truck-at t000-002 l000-000 )
    ( truck-at t000-003 l000-000 )
    ( truck-at t001-000 l001-007 )
    ( truck-at t001-001 l001-002 )
    ( truck-at t001-002 l001-006 )
    ( truck-at t001-003 l001-010 )
    ( truck-at t002-000 l002-000 )
    ( truck-at t002-001 l002-006 )
    ( truck-at t002-002 l002-000 )
    ( truck-at t002-003 l002-003 )
    ( truck-at t003-000 l003-005 )
    ( truck-at t003-001 l003-005 )
    ( truck-at t003-002 l003-003 )
    ( truck-at t004-000 l004-003 )
    ( truck-at t004-001 l004-005 )
    ( truck-at t004-002 l004-006 )
    ( truck-at t004-003 l004-005 )
    ( truck-at t005-000 l005-002 )
    ( truck-at t005-001 l005-002 )
    ( truck-at t005-002 l005-001 )
    ( truck-at t005-003 l005-002 )
    ( truck-at t006-000 l006-004 )
    ( truck-at t006-001 l006-008 )
    ( truck-at t006-002 l006-005 )
    ( truck-at t006-003 l006-001 )
    ( truck-at t007-000 l007-002 )
    ( truck-at t007-001 l007-001 )
    ( truck-at t007-002 l007-001 )
    ( truck-at t007-003 l007-006 )
    ( truck-at t008-000 l008-006 )
    ( truck-at t008-001 l008-000 )
    ( truck-at t008-002 l008-002 )
    ( airplane-at a000 l003-009 )
    ( airplane-at a001 l003-008 )
    ( airplane-at a002 l005-001 )
    ( airplane-at a003 l006-005 )
    ( obj-at p000 l000-004 )
    ( obj-at p001 l001-005 )
    ( obj-at p002 l002-001 )
    ( obj-at p003 l003-006 )
    ( obj-at p004 l004-000 )
    ( obj-at p005 l004-003 )
    ( obj-at p006 l007-001 )
    ( obj-at p007 l006-003 )
    ( obj-at p008 l006-004 )
    ( obj-at p009 l000-001 )
    ( obj-at p010 l001-007 )
    ( obj-at p011 l000-001 )
    ( obj-at p012 l006-003 )
    ( obj-at p013 l004-002 )
    ( obj-at p014 l004-003 )
    ( obj-at p015 l000-000 )
    ( obj-at p016 l001-008 )
    ( obj-at p017 l004-006 )
    ( obj-at p018 l001-001 )
    ( obj-at p019 l002-006 )
    ( obj-at p020 l008-005 )
    ( obj-at p021 l006-008 )
    ( obj-at p022 l008-006 )
    ( obj-at p023 l005-007 )
    ( obj-at p024 l006-001 )
    ( obj-at p025 l000-000 )
    ( obj-at p026 l005-007 )
    ( obj-at p027 l001-004 )
    ( obj-at p028 l005-002 )
    ( obj-at p029 l001-001 )
    ( obj-at p030 l001-003 )
    ( obj-at p031 l004-003 )
    ( obj-at p032 l008-003 )
    ( obj-at p033 l004-007 )
    ( obj-at p034 l000-008 )
    ( obj-at p035 l008-004 )
    ( obj-at p036 l004-002 )
    ( obj-at p037 l000-006 )
    ( obj-at p038 l004-001 )
    ( obj-at p039 l001-006 )
    ( obj-at p040 l000-008 )
    ( obj-at p041 l006-005 )
    ( obj-at p042 l005-007 )
    ( obj-at p043 l000-009 )
    ( obj-at p044 l001-008 )
    ( obj-at p045 l000-004 )
    ( obj-at p046 l007-005 )
    ( obj-at p047 l007-003 )
    ( obj-at p048 l008-005 )
    ( obj-at p049 l000-008 )
    ( obj-at p050 l004-006 )
    ( obj-at p051 l005-000 )
    ( obj-at p052 l003-008 )
    ( obj-at p053 l006-004 )
    ( obj-at p054 l003-009 )
    ( obj-at p055 l004-006 )
    ( obj-at p056 l005-004 )
    ( obj-at p057 l006-007 )
    ( obj-at p058 l006-002 )
    ( obj-at p059 l007-002 )
    ( obj-at p060 l001-000 )
    ( obj-at p061 l000-002 )
    ( obj-at p062 l000-000 )
    ( obj-at p063 l003-006 )
    ( obj-at p064 l001-008 )
    ( obj-at p065 l005-004 )
    ( obj-at p066 l006-008 )
    ( obj-at p067 l006-002 )
    ( obj-at p068 l007-000 )
    ( obj-at p069 l003-009 )
    ( obj-at p070 l005-001 )
    ( obj-at p071 l004-003 )
    ( obj-at p072 l003-006 )
    ( obj-at p073 l007-006 )
    ( obj-at p074 l008-005 )
    ( obj-at p075 l000-009 )
    ( obj-at p076 l004-003 )
    ( obj-at p077 l005-004 )
    ( obj-at p078 l006-008 )
    ( obj-at p079 l007-002 )
  )

  ( :tasks
    ( DELIVER-PKG p000 l006-005 )
    ( DELIVER-PKG p001 l003-006 )
    ( DELIVER-PKG p002 l008-004 )
    ( DELIVER-PKG p003 l008-008 )
    ( DELIVER-PKG p004 l007-009 )
    ( DELIVER-PKG p005 l006-002 )
    ( DELIVER-PKG p006 l002-005 )
    ( DELIVER-PKG p007 l007-001 )
    ( DELIVER-PKG p008 l005-001 )
    ( DELIVER-PKG p009 l005-005 )
    ( DELIVER-PKG p010 l006-005 )
    ( DELIVER-PKG p011 l008-000 )
    ( DELIVER-PKG p012 l001-008 )
    ( DELIVER-PKG p013 l006-003 )
    ( DELIVER-PKG p014 l000-009 )
    ( DELIVER-PKG p015 l000-001 )
    ( DELIVER-PKG p016 l007-001 )
    ( DELIVER-PKG p017 l002-006 )
    ( DELIVER-PKG p018 l000-000 )
    ( DELIVER-PKG p019 l005-005 )
    ( DELIVER-PKG p020 l000-002 )
    ( DELIVER-PKG p021 l003-009 )
    ( DELIVER-PKG p022 l006-001 )
    ( DELIVER-PKG p023 l007-000 )
    ( DELIVER-PKG p024 l004-006 )
    ( DELIVER-PKG p025 l007-007 )
    ( DELIVER-PKG p026 l008-002 )
    ( DELIVER-PKG p027 l008-005 )
    ( DELIVER-PKG p028 l004-003 )
    ( DELIVER-PKG p029 l006-004 )
    ( DELIVER-PKG p030 l008-002 )
    ( DELIVER-PKG p031 l006-005 )
    ( DELIVER-PKG p032 l006-003 )
    ( DELIVER-PKG p033 l003-001 )
    ( DELIVER-PKG p034 l008-005 )
    ( DELIVER-PKG p035 l002-006 )
    ( DELIVER-PKG p036 l001-009 )
    ( DELIVER-PKG p037 l002-000 )
    ( DELIVER-PKG p038 l007-005 )
    ( DELIVER-PKG p039 l002-000 )
    ( DELIVER-PKG p040 l003-007 )
    ( DELIVER-PKG p041 l004-000 )
    ( DELIVER-PKG p042 l001-004 )
    ( DELIVER-PKG p043 l003-008 )
    ( DELIVER-PKG p044 l003-003 )
    ( DELIVER-PKG p045 l006-003 )
    ( DELIVER-PKG p046 l000-006 )
    ( DELIVER-PKG p047 l001-009 )
    ( DELIVER-PKG p048 l003-009 )
    ( DELIVER-PKG p049 l007-003 )
    ( DELIVER-PKG p050 l001-007 )
    ( DELIVER-PKG p051 l006-008 )
    ( DELIVER-PKG p052 l007-009 )
    ( DELIVER-PKG p053 l008-008 )
    ( DELIVER-PKG p054 l008-004 )
    ( DELIVER-PKG p055 l004-001 )
    ( DELIVER-PKG p056 l004-002 )
    ( DELIVER-PKG p057 l006-001 )
    ( DELIVER-PKG p058 l001-001 )
    ( DELIVER-PKG p059 l007-006 )
    ( DELIVER-PKG p060 l003-002 )
    ( DELIVER-PKG p061 l007-008 )
    ( DELIVER-PKG p062 l001-007 )
    ( DELIVER-PKG p063 l008-002 )
    ( DELIVER-PKG p064 l002-002 )
    ( DELIVER-PKG p065 l007-005 )
    ( DELIVER-PKG p066 l002-008 )
    ( DELIVER-PKG p067 l000-007 )
    ( DELIVER-PKG p068 l001-010 )
    ( DELIVER-PKG p069 l008-007 )
    ( DELIVER-PKG p070 l002-008 )
    ( DELIVER-PKG p071 l000-008 )
    ( DELIVER-PKG p072 l002-005 )
    ( DELIVER-PKG p073 l001-008 )
    ( DELIVER-PKG p074 l003-003 )
    ( DELIVER-PKG p075 l008-008 )
    ( DELIVER-PKG p076 l008-001 )
    ( DELIVER-PKG p077 l006-004 )
    ( DELIVER-PKG p078 l001-007 )
    ( DELIVER-PKG p079 l000-004 )
  )
)

