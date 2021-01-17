CREATE TABLE public.email_tokens (
	email VARCHAR(256) NOT NULL,
	user_id INT8 NOT NULL,
	token VARCHAR(64) NOT NULL unique,
	created_at TIMESTAMP NOT NULL DEFAULT now(),
	confirmed BOOL NULL DEFAULT false,
	expired BOOL NULL DEFAULT false
);

CREATE TABLE public.tags (
	id bigserial primary key,
	name VARCHAR(64) NOT NULL,
	topic_count INT8 NULL DEFAULT 0,
	created_at TIMESTAMP NOT NULL DEFAULT now(),
	updated_at TIMESTAMP NOT NULL DEFAULT now()
);

CREATE TABLE public.topic_tags (
	id bigserial primary key,
	topic_id INT8 NOT NULL DEFAULT 0,
	tag_id INT8 NOT NULL DEFAULT 0,
	created_at TIMESTAMP NOT NULL DEFAULT now(),
	updated_at TIMESTAMP NOT NULL DEFAULT now()
);

CREATE TABLE public.topics (
	id bigserial primary key,
	title VARCHAR(512) NOT NULL,
	description VARCHAR(1000000) NOT NULL,
	tag_ids INT8[] NULL,
	posted_by INT8 NOT NULL DEFAULT 0,
	created_at TIMESTAMP NOT NULL DEFAULT now(),
	updated_at TIMESTAMP NOT NULL DEFAULT now(),
	upadted_by INT8 NOT NULL DEFAULT 0,
	visible BOOL NULL DEFAULT true,
	op_id INT8 NOT NULL DEFAULT 0
);

CREATE TABLE public.users (
	id bigserial primary key,
	username VARCHAR(60) NOT NULL,
	created_at TIMESTAMP NOT NULL,
	updated_at TIMESTAMP NOT NULL,
	name VARCHAR NULL,
	seen_notification_id INT8 NOT NULL DEFAULT 0,
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
	views INT8 NOT NULL DEFAULT 0,
	flag_level INT8 NOT NULL DEFAULT 0,
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
	email_verification_code VARCHAR(64) NULL DEFAULT '',
	designation VARCHAR NULL,
	location VARCHAR NULL
);
