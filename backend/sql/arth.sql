--
-- PostgreSQL database dump
--

-- Dumped from database version 12.6 (Ubuntu 12.6-0ubuntu0.20.04.1)
-- Dumped by pg_dump version 12.6 (Ubuntu 12.6-0ubuntu0.20.04.1)

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
-- Name: items; Type: TABLE; Schema: public; Owner: shiv
--

CREATE TABLE public.items (
    n integer,
    s text,
    "time" timestamp with time zone
);


ALTER TABLE public.items OWNER TO shiv;

--
-- Name: tags; Type: TABLE; Schema: public; Owner: shiv
--

CREATE TABLE public.tags (
    id bigint NOT NULL,
    name character varying(64) NOT NULL,
    topic_count bigint DEFAULT 0,
    created_at timestamp without time zone DEFAULT now() NOT NULL,
    updated_at timestamp without time zone DEFAULT now() NOT NULL,
    info character varying DEFAULT ''::character varying
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
    title character varying(512),
    description character varying(1000000) NOT NULL,
    tag_ids bigint[],
    posted_by bigint DEFAULT 0 NOT NULL,
    created_at timestamp without time zone DEFAULT now() NOT NULL,
    updated_at timestamp without time zone DEFAULT now() NOT NULL,
    upadted_by bigint DEFAULT 0 NOT NULL,
    visible boolean DEFAULT true,
    op_id bigint DEFAULT 0,
    updated_by bigint,
    votes bigint DEFAULT 0,
    reply_to bigint,
    slug character varying(256) DEFAULT NULL::character varying,
    views bigint DEFAULT 0
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
    image_url text DEFAULT ''::text,
    git character varying(128) DEFAULT ''::character varying,
    website character varying(256) DEFAULT ''::character varying,
    twitter character varying(128) DEFAULT ''::character varying,
    about_me character varying DEFAULT ''::character varying
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
-- Name: votes; Type: TABLE; Schema: public; Owner: shiv
--

CREATE TABLE public.votes (
    topic_id bigint NOT NULL,
    user_id bigint NOT NULL,
    upvote boolean NOT NULL
);


ALTER TABLE public.votes OWNER TO shiv;

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
-- Name: votes votes_pkey; Type: CONSTRAINT; Schema: public; Owner: shiv
--

ALTER TABLE ONLY public.votes
    ADD CONSTRAINT votes_pkey PRIMARY KEY (topic_id, user_id);


--
-- Name: items_n_idx; Type: INDEX; Schema: public; Owner: shiv
--

CREATE INDEX items_n_idx ON public.items USING btree ("time");


--
-- Name: uatime_idx; Type: INDEX; Schema: public; Owner: shiv
--

CREATE INDEX uatime_idx ON public.topics USING btree (updated_at);


--
-- Name: votes votes_topic_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: shiv
--

ALTER TABLE ONLY public.votes
    ADD CONSTRAINT votes_topic_id_fkey FOREIGN KEY (topic_id) REFERENCES public.topics(id);


--
-- Name: votes votes_user_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: shiv
--

ALTER TABLE ONLY public.votes
    ADD CONSTRAINT votes_user_id_fkey FOREIGN KEY (user_id) REFERENCES public.users(id);


--
-- PostgreSQL database dump complete
--

