CREATE TABLE public.email_tokens (
	email VARCHAR(256) NOT NULL,
	user_id INT8 NOT NULL,
	token VARCHAR(64) NOT NULL,
	created_at TIMESTAMP NOT NULL DEFAULT now():::TIMESTAMP,
	confirmed BOOL NULL DEFAULT false,
	expired BOOL NULL DEFAULT false,
	UNIQUE INDEX email_tokens_token_key (token ASC),
	FAMILY "primary" (email, user_id, token, created_at, rowid, confirmed, expired)
);

CREATE TABLE public.tags (
	id INT8 NOT NULL DEFAULT unique_rowid(),
	name VARCHAR(64) NOT NULL,
	topic_count INT8 NULL DEFAULT 0:::INT8,
	created_at TIMESTAMP NOT NULL DEFAULT now():::TIMESTAMP,
	updated_at TIMESTAMP NOT NULL DEFAULT now():::TIMESTAMP,
	CONSTRAINT "primary" PRIMARY KEY (id ASC),
	FAMILY "primary" (id, name, topic_count, created_at, updated_at)
);

CREATE TABLE public.topic_tags (
	id INT8 NOT NULL DEFAULT unique_rowid(),
	topic_id INT8 NOT NULL DEFAULT unique_rowid(),
	tag_id INT8 NOT NULL DEFAULT unique_rowid(),
	created_at TIMESTAMP NOT NULL,
	updated_at TIMESTAMP NOT NULL,
	CONSTRAINT "primary" PRIMARY KEY (id ASC),
	FAMILY "primary" (id, topic_id, tag_id, created_at, updated_at)
);

CREATE TABLE public.topics (
	id INT8 NOT NULL DEFAULT unique_rowid(),
	title VARCHAR(512) NOT NULL,
	description VARCHAR(1000000) NOT NULL,
	tag_ids INT8[] NULL,
	posted_by INT8 NOT NULL DEFAULT unique_rowid(),
	created_at TIMESTAMP NOT NULL DEFAULT now():::TIMESTAMP,
	updated_at TIMESTAMP NOT NULL DEFAULT now():::TIMESTAMP,
	upadted_by INT8 NOT NULL DEFAULT unique_rowid(),
	visible BOOL NULL DEFAULT true,
	op_id INT8 NOT NULL DEFAULT unique_rowid(),
	CONSTRAINT "primary" PRIMARY KEY (id ASC),
	FAMILY "primary" (id, title, description, tag_ids, posted_by, created_at, updated_at, upadted_by, visible, op_id)
);

CREATE TABLE public.users (
	id INT8 NOT NULL DEFAULT unique_rowid(),
	username VARCHAR(60) NOT NULL,
	created_at TIMESTAMP NOT NULL,
	updated_at TIMESTAMP NOT NULL,
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
	email_verification_code VARCHAR(64) NULL DEFAULT '':::STRING,
	CONSTRAINT users_pkey PRIMARY KEY (id ASC),
	INDEX idx_users_admin (admin ASC) WHERE admin = true,
	UNIQUE INDEX users_emails (email ASC),
	FAMILY "primary" (id, username, created_at, updated_at, name, seen_notification_id, last_posted_at, password_hash, salt, active, username_lower, last_seen_at, admin, last_emailed_at, trust_level, approved, approved_by_id, approved_at, previous_visit_at, suspended_at, suspended_till, date_of_birth, views, flag_level, ip_address, moderator, title, uploaded_avatar_id, locale, primary_group_id, registration_ip_address, staged, first_seen_at, silenced_till, group_locked_trust_level, manual_locked_trust_level, secure_identifier, email, email_verified, email_verification_code)
);
