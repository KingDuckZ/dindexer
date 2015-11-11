--
-- PostgreSQL database dump
--

-- Dumped from database version 9.4.5
-- Dumped by pg_dump version 9.4.5
-- Started on 2015-11-11 02:04:53 GMT

SET statement_timeout = 0;
SET lock_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SET check_function_bodies = false;
SET client_min_messages = warning;

--
-- TOC entry 176 (class 3079 OID 11869)
-- Name: plpgsql; Type: EXTENSION; Schema: -; Owner: 
--

CREATE EXTENSION IF NOT EXISTS plpgsql WITH SCHEMA pg_catalog;


--
-- TOC entry 2022 (class 0 OID 0)
-- Dependencies: 176
-- Name: EXTENSION plpgsql; Type: COMMENT; Schema: -; Owner: 
--

COMMENT ON EXTENSION plpgsql IS 'PL/pgSQL procedural language';


SET search_path = public, pg_catalog;

--
-- TOC entry 189 (class 1255 OID 31290)
-- Name: upcase_hash(); Type: FUNCTION; Schema: public; Owner: michele
--

CREATE FUNCTION upcase_hash() RETURNS trigger
    LANGUAGE plpgsql IMMUTABLE
    AS $$
    BEGIN        
        NEW."hash" = UPPER(NEW."hash");
        RETURN NEW;
    END;
$$;


ALTER FUNCTION public.upcase_hash() OWNER TO michele;

SET default_tablespace = '';

SET default_with_oids = false;

--
-- TOC entry 175 (class 1259 OID 31281)
-- Name: Files; Type: TABLE; Schema: public; Owner: michele; Tablespace: 
--

CREATE TABLE "Files" (
    id bigint NOT NULL,
    path text NOT NULL,
    level smallint NOT NULL,
    group_id integer NOT NULL,
    is_directory boolean NOT NULL,
    is_symlink boolean NOT NULL,
    size bigint,
    hash character(48) NOT NULL
);


ALTER TABLE "Files" OWNER TO michele;

--
-- TOC entry 174 (class 1259 OID 31279)
-- Name: Files_id_seq; Type: SEQUENCE; Schema: public; Owner: michele
--

CREATE SEQUENCE "Files_id_seq"
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE "Files_id_seq" OWNER TO michele;

--
-- TOC entry 2023 (class 0 OID 0)
-- Dependencies: 174
-- Name: Files_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: michele
--

ALTER SEQUENCE "Files_id_seq" OWNED BY "Files".id;


--
-- TOC entry 1897 (class 2604 OID 31284)
-- Name: id; Type: DEFAULT; Schema: public; Owner: michele
--

ALTER TABLE ONLY "Files" ALTER COLUMN id SET DEFAULT nextval('"Files_id_seq"'::regclass);


--
-- TOC entry 2014 (class 0 OID 31281)
-- Dependencies: 175
-- Data for Name: Files; Type: TABLE DATA; Schema: public; Owner: michele
--

COPY "Files" (id, path, level, group_id, is_directory, is_symlink, size, hash) FROM stdin;
\.


--
-- TOC entry 2024 (class 0 OID 0)
-- Dependencies: 174
-- Name: Files_id_seq; Type: SEQUENCE SET; Schema: public; Owner: michele
--

SELECT pg_catalog.setval('"Files_id_seq"', 128, true);


--
-- TOC entry 1900 (class 2606 OID 31289)
-- Name: pk_files_id; Type: CONSTRAINT; Schema: public; Owner: michele; Tablespace: 
--

ALTER TABLE ONLY "Files"
    ADD CONSTRAINT pk_files_id PRIMARY KEY (id);


--
-- TOC entry 1902 (class 2606 OID 31294)
-- Name: uniq_item; Type: CONSTRAINT; Schema: public; Owner: michele; Tablespace: 
--

ALTER TABLE ONLY "Files"
    ADD CONSTRAINT uniq_item UNIQUE (path, group_id);


--
-- TOC entry 1898 (class 1259 OID 31292)
-- Name: idx_paths; Type: INDEX; Schema: public; Owner: michele; Tablespace: 
--

CREATE INDEX idx_paths ON "Files" USING btree (path);


--
-- TOC entry 1903 (class 2620 OID 31291)
-- Name: triggerupcasehash; Type: TRIGGER; Schema: public; Owner: michele
--

CREATE TRIGGER triggerupcasehash BEFORE INSERT OR UPDATE ON "Files" FOR EACH ROW EXECUTE PROCEDURE upcase_hash();


--
-- TOC entry 2021 (class 0 OID 0)
-- Dependencies: 8
-- Name: public; Type: ACL; Schema: -; Owner: postgres
--

REVOKE ALL ON SCHEMA public FROM PUBLIC;
REVOKE ALL ON SCHEMA public FROM postgres;
GRANT ALL ON SCHEMA public TO postgres;
GRANT ALL ON SCHEMA public TO PUBLIC;


-- Completed on 2015-11-11 02:04:55 GMT

--
-- PostgreSQL database dump complete
--

