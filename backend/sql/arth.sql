--
-- PostgreSQL database dump
--

-- Dumped from database version 13.1 (Ubuntu 13.1-1.pgdg20.04+1)
-- Dumped by pg_dump version 13.1 (Ubuntu 13.1-1.pgdg20.04+1)

SET statement_timeout = 0;
SET lock_timeout = 0;
SET idle_in_transaction_session_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SELECT pg_catalog.set_config('search_path', '', false);
SET check_function_bodies = false;
SET xmloption = content;
SET client_min_messages = warning;
SET row_security = off;

SET default_tablespace = '';

SET default_table_access_method = heap;

--
-- Name: email_tokens; Type: TABLE; Schema: public; Owner: shiv
--

CREATE TABLE public.email_tokens (
    email character varying(256) NOT NULL,
    user_id bigint NOT NULL,
    token character varying(64) NOT NULL,
    created_at timestamp without time zone DEFAULT now() NOT NULL,
    confirmed boolean DEFAULT false,
    expired boolean DEFAULT false
);


ALTER TABLE public.email_tokens OWNER TO shiv;

--
-- Name: tags; Type: TABLE; Schema: public; Owner: shiv
--

CREATE TABLE public.tags (
    id bigint NOT NULL,
    name character varying(64) NOT NULL,
    topic_count bigint DEFAULT 0,
    created_at timestamp without time zone DEFAULT now() NOT NULL,
    updated_at timestamp without time zone DEFAULT now() NOT NULL
);


ALTER TABLE public.tags OWNER TO shiv;

--
-- Name: tags_id_seq; Type: SEQUENCE; Schema: public; Owner: shiv
--

CREATE SEQUENCE public.tags_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.tags_id_seq OWNER TO shiv;

--
-- Name: tags_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: shiv
--

ALTER SEQUENCE public.tags_id_seq OWNED BY public.tags.id;


--
-- Name: topic_tags; Type: TABLE; Schema: public; Owner: shiv
--

CREATE TABLE public.topic_tags (
    id bigint NOT NULL,
    topic_id bigint DEFAULT 0 NOT NULL,
    tag_id bigint DEFAULT 0 NOT NULL,
    created_at timestamp without time zone DEFAULT now() NOT NULL,
    updated_at timestamp without time zone DEFAULT now() NOT NULL
);


ALTER TABLE public.topic_tags OWNER TO shiv;

--
-- Name: topic_tags_id_seq; Type: SEQUENCE; Schema: public; Owner: shiv
--

CREATE SEQUENCE public.topic_tags_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.topic_tags_id_seq OWNER TO shiv;

--
-- Name: topic_tags_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: shiv
--

ALTER SEQUENCE public.topic_tags_id_seq OWNED BY public.topic_tags.id;


--
-- Name: topics; Type: TABLE; Schema: public; Owner: shiv
--

CREATE TABLE public.topics (
    id bigint NOT NULL,
    title character varying(512) NOT NULL,
    description character varying(1000000) NOT NULL,
    tag_ids bigint[],
    posted_by bigint DEFAULT 0 NOT NULL,
    created_at timestamp without time zone DEFAULT now() NOT NULL,
    updated_at timestamp without time zone DEFAULT now() NOT NULL,    
    visible boolean DEFAULT true,
    op_id bigint DEFAULT 0 NOT NULL,
    updated_by bigint
);


ALTER TABLE public.topics OWNER TO shiv;

--
-- Name: topics_id_seq; Type: SEQUENCE; Schema: public; Owner: shiv
--

CREATE SEQUENCE public.topics_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.topics_id_seq OWNER TO shiv;

--
-- Name: topics_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: shiv
--

ALTER SEQUENCE public.topics_id_seq OWNED BY public.topics.id;


--
-- Name: users; Type: TABLE; Schema: public; Owner: shiv
--

CREATE TABLE public.users (
    id bigint NOT NULL,
    username character varying(60) NOT NULL,
    created_at timestamp without time zone DEFAULT now() NOT NULL,
    updated_at timestamp without time zone DEFAULT now() NOT NULL,
    name character varying,
    seen_notification_id bigint DEFAULT 0 NOT NULL,
    last_posted_at timestamp without time zone,
    password_hash character varying(256),
    salt character varying(64),
    active boolean DEFAULT false NOT NULL,
    username_lower character varying(60) NOT NULL,
    last_seen_at timestamp without time zone,
    admin boolean DEFAULT false NOT NULL,
    last_emailed_at timestamp without time zone,
    trust_level bigint NOT NULL,
    approved boolean DEFAULT false NOT NULL,
    approved_by_id bigint,
    approved_at timestamp without time zone,
    previous_visit_at timestamp without time zone,
    suspended_at timestamp without time zone,
    suspended_till timestamp without time zone,
    date_of_birth date,
    views bigint DEFAULT 0 NOT NULL,
    flag_level bigint DEFAULT 0 NOT NULL,
    ip_address inet,
    moderator boolean DEFAULT false,
    title character varying,
    uploaded_avatar_id bigint,
    locale character varying(10),
    primary_group_id bigint,
    registration_ip_address inet,
    staged boolean DEFAULT false NOT NULL,
    first_seen_at timestamp without time zone,
    silenced_till timestamp without time zone,
    group_locked_trust_level bigint,
    manual_locked_trust_level bigint,
    secure_identifier character varying,
    email character varying(255) NOT NULL,
    email_verified boolean DEFAULT false,
    email_verification_code character varying(64) DEFAULT ''::character varying,
    designation character varying,
    location character varying,
    image_url text DEFAULT ''::text
);


ALTER TABLE public.users OWNER TO shiv;

--
-- Name: users_id_seq; Type: SEQUENCE; Schema: public; Owner: shiv
--

CREATE SEQUENCE public.users_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.users_id_seq OWNER TO shiv;

--
-- Name: users_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: shiv
--

ALTER SEQUENCE public.users_id_seq OWNED BY public.users.id;


--
-- Name: tags id; Type: DEFAULT; Schema: public; Owner: shiv
--

ALTER TABLE ONLY public.tags ALTER COLUMN id SET DEFAULT nextval('public.tags_id_seq'::regclass);


--
-- Name: topic_tags id; Type: DEFAULT; Schema: public; Owner: shiv
--

ALTER TABLE ONLY public.topic_tags ALTER COLUMN id SET DEFAULT nextval('public.topic_tags_id_seq'::regclass);


--
-- Name: topics id; Type: DEFAULT; Schema: public; Owner: shiv
--

ALTER TABLE ONLY public.topics ALTER COLUMN id SET DEFAULT nextval('public.topics_id_seq'::regclass);


--
-- Name: users id; Type: DEFAULT; Schema: public; Owner: shiv
--

ALTER TABLE ONLY public.users ALTER COLUMN id SET DEFAULT nextval('public.users_id_seq'::regclass);


--
-- Data for Name: email_tokens; Type: TABLE DATA; Schema: public; Owner: shiv
--

COPY public.email_tokens (email, user_id, token, created_at, confirmed, expired) FROM stdin;
\.


--
-- Data for Name: tags; Type: TABLE DATA; Schema: public; Owner: shiv
--

COPY public.tags (id, name, topic_count, created_at, updated_at) FROM stdin;
1	c	0	2021-01-18 06:17:16.301578	2021-01-18 06:17:16.301578
2	c++	0	2021-01-18 06:17:19.71306	2021-01-18 06:17:19.71306
4	java	0	2021-01-18 06:17:26.843186	2021-01-18 06:17:26.843186
5	javascript	0	2021-01-18 06:17:31.900998	2021-01-18 06:17:31.900998
3	rust	1	2021-01-18 06:17:23.220869	2021-01-18 06:17:23.220869
\.


--
-- Data for Name: topic_tags; Type: TABLE DATA; Schema: public; Owner: shiv
--

COPY public.topic_tags (id, topic_id, tag_id, created_at, updated_at) FROM stdin;
1	1	3	2021-01-18 06:19:38.040455	2021-01-18 06:19:38.040455
\.


--
-- Data for Name: topics; Type: TABLE DATA; Schema: public; Owner: shiv
--

COPY public.topics (id, title, description, tag_ids, posted_by, created_at, updated_at, upadted_by, visible, op_id, updated_by) FROM stdin;
1	This is my first post	This is my first postThis is my first postThis is my first postThis is my first postThis is my first postThis is my first postThis is my first post	\N	2	2021-01-18 06:19:38.040455	2021-01-18 06:19:38.040455	0	t	0	2
\.


--
-- Data for Name: users; Type: TABLE DATA; Schema: public; Owner: shiv
--

COPY public.users (id, username, created_at, updated_at, name, seen_notification_id, last_posted_at, password_hash, salt, active, username_lower, last_seen_at, admin, last_emailed_at, trust_level, approved, approved_by_id, approved_at, previous_visit_at, suspended_at, suspended_till, date_of_birth, views, flag_level, ip_address, moderator, title, uploaded_avatar_id, locale, primary_group_id, registration_ip_address, staged, first_seen_at, silenced_till, group_locked_trust_level, manual_locked_trust_level, secure_identifier, email, email_verified, email_verification_code, designation, location, image_url) FROM stdin;
2	shiv	2021-01-17 12:49:16.707607	2021-01-17 12:49:16.707607	\N	0	\N	$argon2i$v=19$m=262144,t=4,p=1$MzgxMTExODcxMzUxNTIzMTgwMjAyMjA2MjI1MjQ1MjIxMjE2MTg2MjE2MTE4$zShBV2WHZOYk3vMqic9dJqswdFdlsXymkuR9SCIBlUYkr7QkAsW3RccNxc894z49JUyE4SmfuCulkAARqunFoQ	381111871351523180202206225245221216186216118	f	shiv	\N	f	\N	0	f	\N	\N	\N	\N	\N	\N	0	0	\N	f	\N	\N	\N	\N	\N	f	\N	\N	\N	\N	\N	shivshankar.dayal@gmail.com	t		\N	\N	
\.


--
-- Name: tags_id_seq; Type: SEQUENCE SET; Schema: public; Owner: shiv
--

SELECT pg_catalog.setval('public.tags_id_seq', 5, true);


--
-- Name: topic_tags_id_seq; Type: SEQUENCE SET; Schema: public; Owner: shiv
--

SELECT pg_catalog.setval('public.topic_tags_id_seq', 1, true);


--
-- Name: topics_id_seq; Type: SEQUENCE SET; Schema: public; Owner: shiv
--

SELECT pg_catalog.setval('public.topics_id_seq', 1, true);


--
-- Name: users_id_seq; Type: SEQUENCE SET; Schema: public; Owner: shiv
--

SELECT pg_catalog.setval('public.users_id_seq', 2, true);


--
-- Name: email_tokens email_tokens_token_key; Type: CONSTRAINT; Schema: public; Owner: shiv
--

ALTER TABLE ONLY public.email_tokens
    ADD CONSTRAINT email_tokens_token_key UNIQUE (token);


--
-- Name: tags tags_pkey; Type: CONSTRAINT; Schema: public; Owner: shiv
--

ALTER TABLE ONLY public.tags
    ADD CONSTRAINT tags_pkey PRIMARY KEY (id);


--
-- Name: topic_tags topic_tags_pkey; Type: CONSTRAINT; Schema: public; Owner: shiv
--

ALTER TABLE ONLY public.topic_tags
    ADD CONSTRAINT topic_tags_pkey PRIMARY KEY (id);


--
-- Name: topics topics_pkey; Type: CONSTRAINT; Schema: public; Owner: shiv
--

ALTER TABLE ONLY public.topics
    ADD CONSTRAINT topics_pkey PRIMARY KEY (id);


--
-- Name: users users_pkey; Type: CONSTRAINT; Schema: public; Owner: shiv
--

ALTER TABLE ONLY public.users
    ADD CONSTRAINT users_pkey PRIMARY KEY (id);


--
-- PostgreSQL database dump complete
--

