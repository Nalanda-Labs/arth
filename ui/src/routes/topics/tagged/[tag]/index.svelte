<script context="module">
    export function preload({ params }) {
        let tag = params.tag;
        console.log(tag);
        return {tag};
    }
</script>

<script>
    import { goto, stores } from "@sapper/app";
    import Button, { Label } from "@smui/button";
    import { onMount } from "svelte";
    import * as api from "api.js";
    import InfiniteLoading from "svelte-infinite-loading";

    export let tag;
    let topics = [];
    let data = [];
    let count = 0;

    async function fetchData(passed_tag="") {
        console.log(tag);
        data = [];
        topics = [];
        if(passed_tag !== "") {
            tag = passed_tag;
        }
        let updated_at = "";
        if (topics.length) {
            updated_at = topics[topics.length - 1].updated_at;
        }
        if(passed_tag === "") {
            updated_at = "";
        }
        let response = await api.post(`topics/tagged/${tag}`, {
            updated_at: updated_at,
        });

        if (response.topics) {
            topics = response.topics;
        }
        for (let i = 0; i < topics.length; i++) {
            topics[i]["tags"] = topics[i]["tags"].slice(1, -1);
            topics[i]["tags"] = topics[i]["tags"].split(",");
            topics[i]["tid"] = topics[i]["tid"].slice(1, -1);
            topics[i]["tid"] = topics[i]["tid"].split(",");
            let asked_ts = Date.parse(topics[i].created_at);
            let updated_ts = Date.parse(topics[i].updated_at);
            let now = new Date();

            if (asked_ts == updated_ts) {
                var offset = new Date().getTimezoneOffset();
                let shown_ts = Math.floor(
                    (now - asked_ts) / 1000 + offset * 60
                );
                if (shown_ts >= 259200) {
                    asked_ts = new Date(topics[i].created_at);
                    let year = asked_ts.getYear() + 1900;
                    let month = asked_ts.getMonth() + 1;
                    shown_ts =
                        "asked on " +
                        asked_ts.getDate() +
                        "/" +
                        month +
                        "/" +
                        year;
                } else if (172800 <= shown_ts && shown_ts < 259200) {
                    shown_ts = "asked 2 days ago";
                } else if (86400 <= shown_ts && shown_ts < 172800) {
                    shown_ts = "asked yesterday";
                } else if (3600 <= shown_ts && shown_ts < 8640000) {
                    shown_ts = "asked " + Math.floor(shown_ts / 3600) + "h ago";
                } else if (60 <= shown_ts && shown_ts < 3600) {
                    console.log(shown_ts);
                    shown_ts = "asked " + Math.floor(shown_ts / 60) + "m ago";
                } else {
                    shown_ts = "asked " + shown_ts + "s ago";
                }
                topics[i].shown_ts = shown_ts;
            } else {
                var offset = new Date().getTimezoneOffset();
                let shown_ts = Math.floor(
                    (now - updated_ts) / 1000 + offset * 60
                );
                if (shown_ts >= 259200) {
                    asked_ts = new Date(topics[i].created_at);
                    let year = updated_ts.getYear() + 1900;
                    let month = updated_ts.getMonth() + 1;
                    shown_ts =
                        "modified on " +
                        updated_ts.getDay() +
                        "/" +
                        month +
                        "/" +
                        year;
                } else if (172800 <= shown_ts && shown_ts < 259200) {
                    shown_ts = "modiffed 2 days ago";
                } else if (86400 <= shown_ts && shown_ts < 172800) {
                    shown_ts = "modified yesterday";
                } else if (3600 <= shown_ts && shown_ts < 8640000) {
                    shown_ts =
                        "modified " + Math.floor(shown_ts / 3600) + "h ago";
                } else if (60 <= shown_ts && shown_ts < 3600) {
                    console.log(shown_ts);
                    shown_ts =
                        "modified " + Math.floor(shown_ts / 60) + "m ago";
                } else {
                    shown_ts = "modified " + shown_ts + "s ago";
                }
                topics[i].shown_ts = shown_ts;
            }
        }
        if (topics.length) {
            data = topics;
        }
    }
    onMount(async () => {
        await fetchData();
    });
    // $: data = [...data, ...topics];

    async function infiniteHandler({ detail: { loaded, complete } }) {
        let updated_at = "";
        if (topics.length) {
            updated_at = topics[topics.length - 1].updated_at;
        }
        let response = await api.post(`topics/tagged/${tag}`, {
            updated_at: updated_at,
        });
        if (response.topics) {
            topics = response.topics;
        }
        if (topics.length) {
            for (let i = 0; i < topics.length; i++) {
                topics[i]["tags"] = topics[i]["tags"].slice(1, -1);
                topics[i]["tags"] = topics[i]["tags"].split(",");
                topics[i]["tid"] = topics[i]["tid"].slice(1, -1);
                topics[i]["tid"] = topics[i]["tid"].split(",");
                let asked_ts = Date.parse(topics[i].created_at);
                let updated_ts = Date.parse(topics[i].updated_at);
                let now = new Date();

                if (asked_ts == updated_ts) {
                    var offset = new Date().getTimezoneOffset();
                    let shown_ts = Math.floor(
                        (now - asked_ts) / 1000 + offset * 60
                    );
                    if (shown_ts >= 259200) {
                        asked_ts = new Date(topics[i].created_at);
                        let year = asked_ts.getYear() + 1900;
                        let month = asked_ts.getMonth() + 1;
                        shown_ts =
                            "asked on " +
                            asked_ts.getDate() +
                            "/" +
                            month +
                            "/" +
                            year;
                    } else if (172800 <= shown_ts && shown_ts < 259200) {
                        shown_ts = "asked 2 days ago";
                    } else if (86400 <= shown_ts && shown_ts < 172800) {
                        shown_ts = "asked yesterday";
                    } else if (3600 <= shown_ts && shown_ts < 8640000) {
                        shown_ts =
                            "asked " + Math.floor(shown_ts / 3600) + "h ago";
                    } else if (60 <= shown_ts && shown_ts < 3600) {
                        console.log(shown_ts);
                        shown_ts =
                            "asked " + Math.floor(shown_ts / 60) + "m ago";
                    } else {
                        shown_ts = "asked " + shown_ts + "s ago";
                    }
                    topics[i].shown_ts = shown_ts;
                } else {
                    var offset = new Date().getTimezoneOffset();
                    let shown_ts = Math.floor(
                        (now - updated_ts) / 1000 + offset * 60
                    );
                    if (shown_ts >= 259200) {
                        asked_ts = new Date(topics[i].created_at);
                        let year = updated_ts.getYear() + 1900;
                        let month = updated_ts.getMonth() + 1;
                        shown_ts =
                            "modified on " +
                            updated_ts.getDay() +
                            "/" +
                            month +
                            "/" +
                            year;
                    } else if (172800 <= shown_ts && shown_ts < 259200) {
                        shown_ts = "modiffed 2 days ago";
                    } else if (86400 <= shown_ts && shown_ts < 172800) {
                        shown_ts = "modified yesterday";
                    } else if (3600 <= shown_ts && shown_ts < 8640000) {
                        shown_ts =
                            "modified " + Math.floor(shown_ts / 3600) + "h ago";
                    } else if (60 <= shown_ts && shown_ts < 3600) {
                        console.log(shown_ts);
                        shown_ts =
                            "modified " + Math.floor(shown_ts / 60) + "m ago";
                    } else {
                        shown_ts = "modified " + shown_ts + "s ago";
                    }
                    topics[i].shown_ts = shown_ts;
                }
            }
            data = [...data, ...topics];
            loaded();
        } else {
            complete();
        }
    }
</script>

<svelte:head>
    <title>All Topics • Arth</title>
</svelte:head>
<div class="topic container">
    <h3>
        Questions tagged [{tag}]
        <Button
            on:click={() => goto("/ask")}
            variant="raised"
            style="float:right;margin-right:50px;margin-top:20px"
        >
            <Label>Post topic</Label>
        </Button>
    </h3>
    <a href="/tags/edit/{tag}" style="float:right;text-decoration:none; color: #4285F4;">Improve Tag Info</a>
    <a href="/tags/{tag}" style="float:right;text-decoration:none; color: #4285F4;margin-right:10px">Tag Info</a>
    <div class="row" style="width:100%">
        {#each data as { id, slug, title, tags, shown_ts, uid, username, answers, views }}
            <hr
                style="border-bottom:1px solid;color:#eee;display:block;min-width:100%;margin-top:20px;margin-bottom:20px"
            />
            <div
                style="margin-right:10px;flex-basis: 5%;max-width:5%;height:60px"
                class="mdc-elevation--z4"
            >
                <p style="text-align:center;font-size:16px;margin-top:5px">
                    {answers}
                </p>
                <p style="text-align:center;font-size:9px;margin-top:-20px;">
                    answers
                </p>
            </div>
            <div
                style="margin-right:10px;flex-basis: 5%;max-width:5%;height:60px"
                class="mdc-elevation--z4"
            >
                <p style="text-align:center;font-size:16px;margin-top:5px">
                    {views}
                </p>
                <p style="text-align:center;font-size:9px;margin-top:-20px;">
                    views
                </p>
            </div>
            <div style="width:85%;float:left;position:relative">
                <a
                    href="/t/{id}/{slug}"
                    style="text-decoration:none; color: #4285F4; font-size:16px; font-weight:400"
                    >{title}</a
                >
                <div style="margin-top:10px;clear:both" />
                {#each tags as tag, i}
                    <a
                        href="/topics/tagged/{tag}"
                        style="text-decoration:none; color: #fff;background-color: #4285F4; padding:7px; margin-right:10px; border-radius: 4px"
                        on:click={fetchData(tag)}>{tag}</a
                    >
                {/each}
                <span style="float:right"
                    >{shown_ts}
                    <a
                        href="/users/{uid}/{username}"
                        style="text-decoration:none; color:#4285F4;"
                        >{username}</a
                    ></span
                >
            </div>
        {/each}
        <InfiniteLoading on:infinite={infiniteHandler} />
        <hr
            style="border-bottom:1px solid;color:#eee;display:block;min-width:100%;margin-top:20px"
        />
    </div>
</div>

<style>
    @media (max-width: 720px) {
        .topic {
            width: 100%;
        }
    }
    @media (max-width: 4096px) {
        .topic {
            width: 800px;
        }
        :global(.bytemd-editor .CodeMirror) {
            height: 90% !important;
        }
        .container {
            margin-left: 10px;
        }
    }
</style>
