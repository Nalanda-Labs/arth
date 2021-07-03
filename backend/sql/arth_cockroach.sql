CREATE TABLE public.email_tokens (
	email VARCHAR(256) NOT NULL,
	user_id INT8 NOT NULL,
	token VARCHAR(64) NOT NULL,
	created_at TIMESTAMP NOT NULL DEFAULT now():::TIMESTAMP,
	confirmed BOOL NULL DEFAULT false,
	expired BOOL NULL DEFAULT false,
	rowid INT8 NOT VISIBLE NOT NULL DEFAULT unique_rowid(),
	CONSTRAINT "primary" PRIMARY KEY (rowid ASC),
	UNIQUE INDEX email_tokens_token_key (token ASC),
	FAMILY "primary" (email, user_id, token, created_at, confirmed, expired, rowid)
);
CREATE TABLE public.items (
	n INT8 NULL,
	s STRING NULL,
	"time" TIMESTAMPTZ NULL,
	rowid INT8 NOT VISIBLE NOT NULL DEFAULT unique_rowid(),
	CONSTRAINT "primary" PRIMARY KEY (rowid ASC),
	INDEX items_n_idx ("time" ASC),
	FAMILY "primary" (n, s, "time", rowid)
);
CREATE SEQUENCE public.tags_id_seq MINVALUE 1 MAXVALUE 9223372036854775807 INCREMENT 1 START 1;
CREATE TABLE public.tags (
	id INT8 NOT NULL DEFAULT nextval('public.tags_id_seq':::STRING::REGCLASS),
	name VARCHAR(64) NOT NULL,
	topic_count INT8 NULL DEFAULT 0:::INT8,
	created_at TIMESTAMP NOT NULL DEFAULT now():::TIMESTAMP,
	updated_at TIMESTAMP NOT NULL DEFAULT now():::TIMESTAMP,
	info VARCHAR NULL DEFAULT '':::STRING::VARCHAR,
	rowid INT8 NOT VISIBLE NOT NULL DEFAULT unique_rowid(),
	CONSTRAINT tags_pkey PRIMARY KEY (id ASC),
	FAMILY "primary" (id, name, topic_count, created_at, updated_at, info, rowid)
);
CREATE SEQUENCE public.topic_tags_id_seq MINVALUE 1 MAXVALUE 9223372036854775807 INCREMENT 1 START 1;
CREATE TABLE public.topic_tags (
	id INT8 NOT NULL DEFAULT nextval('public.topic_tags_id_seq':::STRING::REGCLASS),
	topic_id INT8 NOT NULL DEFAULT 0:::INT8,
	tag_id INT8 NOT NULL DEFAULT 0:::INT8,
	created_at TIMESTAMP NOT NULL DEFAULT now():::TIMESTAMP,
	updated_at TIMESTAMP NOT NULL DEFAULT now():::TIMESTAMP,
	rowid INT8 NOT VISIBLE NOT NULL DEFAULT unique_rowid(),
	CONSTRAINT topic_tags_pkey PRIMARY KEY (id ASC),
	FAMILY "primary" (id, topic_id, tag_id, created_at, updated_at, rowid)
);
CREATE SEQUENCE public.topics_id_seq MINVALUE 1 MAXVALUE 9223372036854775807 INCREMENT 1 START 1;
CREATE TABLE public.topics (
	id INT8 NOT NULL DEFAULT nextval('public.topics_id_seq':::STRING::REGCLASS),
	title VARCHAR(512) NULL,
	description VARCHAR(1000000) NOT NULL,
	tag_ids INT8[] NULL,
	posted_by INT8 NOT NULL DEFAULT 0:::INT8,
	created_at TIMESTAMP NOT NULL DEFAULT now():::TIMESTAMP,
	updated_at TIMESTAMP NOT NULL DEFAULT now():::TIMESTAMP,
	upadted_by INT8 NOT NULL DEFAULT 0:::INT8,
	visible1 BOOL NULL DEFAULT true,
	op_id INT8 NULL DEFAULT 0:::INT8,
	updated_by INT8 NULL,
	votes INT8 NULL DEFAULT 0:::INT8,
	reply_to INT8 NULL,
	slug VARCHAR(256) NULL DEFAULT NULL::VARCHAR,
	views INT8 NULL DEFAULT 0:::INT8,
	rowid INT8 NOT VISIBLE NOT NULL DEFAULT unique_rowid(),
	CONSTRAINT topics_pkey PRIMARY KEY (id ASC),
	INDEX uatime_idx (updated_at ASC),
	FAMILY "primary" (id, title, description, tag_ids, posted_by, created_at, updated_at, upadted_by, visible1, op_id, updated_by, votes, reply_to, slug, views, rowid)
);
CREATE SEQUENCE public.users_id_seq MINVALUE 1 MAXVALUE 9223372036854775807 INCREMENT 1 START 1;
CREATE TABLE public.users (
	id INT8 NOT NULL DEFAULT nextval('public.users_id_seq':::STRING::REGCLASS),
	username VARCHAR(60) NOT NULL,
	created_at TIMESTAMP NOT NULL DEFAULT now():::TIMESTAMP,
	updated_at TIMESTAMP NOT NULL DEFAULT now():::TIMESTAMP,
	name VARCHAR NULL,
	seen_notification_id INT8 NOT NULL DEFAULT 0:::INT8,
	last_posted_at TIMESTAMP NULL,
	password_hash VARCHAR(256) NULL,
	salt VARCHAR(64) NULL,
	active BOOL NOT NULL DEFAULT false,
	username_lower VARCHAR(60) NOT NULL,
	last_seen_at TIMESTAMP NULL,
	admin BOOL NOT NULL DEFAULT false,
	last_emailed_at TIMESTAMP NULL,
	trust_level INT8 NOT NULL,
	approved BOOL NOT NULL DEFAULT false,
	approved_by_id INT8 NULL,
	approved_at TIMESTAMP NULL,
	previous_visit_at TIMESTAMP NULL,
	suspended_at TIMESTAMP NULL,
	suspended_till TIMESTAMP NULL,
	date_of_birth DATE NULL,
	views INT8 NOT NULL DEFAULT 0:::INT8,
	flag_level INT8 NOT NULL DEFAULT 0:::INT8,
	ip_address INET NULL,
	moderator BOOL NULL DEFAULT false,
	title VARCHAR NULL,
	uploaded_avatar_id INT8 NULL,
	locale VARCHAR(10) NULL,
	primary_group_id INT8 NULL,
	registration_ip_address INET NULL,
	staged BOOL NOT NULL DEFAULT false,
	first_seen_at TIMESTAMP NULL,
	silenced_till TIMESTAMP NULL,
	group_locked_trust_level INT8 NULL,
	manual_locked_trust_level INT8 NULL,
	secure_identifier VARCHAR NULL,
	email VARCHAR(255) NOT NULL,
	email_verified BOOL NULL DEFAULT false,
	email_verification_code VARCHAR(64) NULL DEFAULT '':::STRING::VARCHAR,
	designation VARCHAR NULL,
	location VARCHAR NULL,
	image_url STRING NULL DEFAULT '':::STRING,
	git VARCHAR(128) NULL DEFAULT '':::STRING::VARCHAR,
	website VARCHAR(256) NULL DEFAULT '':::STRING::VARCHAR,
	twitter VARCHAR(128) NULL DEFAULT '':::STRING::VARCHAR,
	about_me VARCHAR NULL DEFAULT '':::STRING::VARCHAR,
	rowid INT8 NOT VISIBLE NOT NULL DEFAULT unique_rowid(),
	CONSTRAINT users_pkey PRIMARY KEY (id ASC),
	FAMILY "primary" (id, username, created_at, updated_at, name, seen_notification_id, last_posted_at, password_hash, salt, active, username_lower, last_seen_at, admin, last_emailed_at, trust_level, approved, approved_by_id, approved_at, previous_visit_at, suspended_at, suspended_till, date_of_birth, views, flag_level, ip_address, moderator, title, uploaded_avatar_id, locale, primary_group_id, registration_ip_address, staged, first_seen_at, silenced_till, group_locked_trust_level, manual_locked_trust_level, secure_identifier, email, email_verified, email_verification_code, designation, location, image_url, git, website, twitter, about_me, rowid)
);
CREATE TABLE public.votes (
	topic_id INT8 NOT NULL,
	user_id INT8 NOT NULL,
	upvote BOOL NOT NULL,
	rowid INT8 NOT VISIBLE NOT NULL DEFAULT unique_rowid(),
	CONSTRAINT votes_pkey PRIMARY KEY (topic_id ASC, user_id ASC),
	FAMILY "primary" (topic_id, user_id, upvote, rowid)
);
ALTER TABLE public.votes ADD CONSTRAINT votes_topic_id_fkey FOREIGN KEY (topic_id) REFERENCES public.topics(id);
ALTER TABLE public.votes ADD CONSTRAINT votes_user_id_fkey FOREIGN KEY (user_id) REFERENCES public.users(id);
-- Validate foreign key constraints. These can fail if there was unvalidated data during the SHOW CREATE ALL TABLES
ALTER TABLE public.votes VALIDATE CONSTRAINT votes_topic_id_fkey;
ALTER TABLE public.votes VALIDATE CONSTRAINT votes_user_id_fkey;
