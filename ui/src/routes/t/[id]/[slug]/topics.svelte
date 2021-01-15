<script>
    import { onMount } from "svelte";
    import * as api from "api.js";
    import "bytemd/dist/index.min.css";
    import TagList from "../../../_components/TagList.svelte";

    export let id;
    export let slug;

    let topics = [];
    let title = "";
    let tagList = [];
    let op_body = "";
    let Viewer = null;

    onMount(async () => {
        const bytemd = await import("bytemd");
        Viewer = bytemd.Viewer;

        const response = await api.get(
            `t/${id}/${slug}`,
            localStorage.getItem("jwt")
        );

        if (response.topics) {
            topics = response.topics;
            title = response.title;
            tagList = response.tagLIst;
            op_body = topics[0].description;
        }
    });
</script>

<div>
    <h3>{title}</h3>
    <svelte:component this={Viewer} {op_body} />
    <TagList {tagList} />
</div>
