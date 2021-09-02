<script context="module">
	export function preload({ params }, { user }) {
		let page = params.page;
		return { page };
	}
</script>

<script>
	import { onMount } from "svelte";
	import * as api from "api.js";
	import Card, { Content } from "@smui/card";
	import "../_utils.scss";

	export let page;
	if (!page) {
		page = 1;
	}
	let currentPage = page;
	let tags = [];
	let count = 0;

	onMount(async () => {
		if (!page) {
			page = 1;
		}

		let response = await api.get(`tags/`);
		if (response.tags) {
			tags = response.tags;
			if(tags.length > 0) {
				count = tags[0].count;
			}
		}
	});
</script>

<svelte:head>
	<title>Tags • Arth</title>
</svelte:head>

<div class="tags">
	<h3>Tags</h3>
	<p>
		A tag lets you know technologies, concepts involved with a
		topic/question. It helps you find similar questions or watch a
		particular tag for topics/questions.
	</p>
	<div class="row">
		{#each tags as { id, info, name, topic_count }}
			<Card
				style="width:150px;height:150px;margin-right:20px;margin-top:10px;margin-left:10px"
			>
				<a
					class="anchor"
					style="margin-left:10px;font-size:20px;margin-top:10px;border-bottom:1px solid #ddd"
					href="/topics/tagged/{name}">{name}</a
				>
				<span style="margin-left:10px;float:right"
					>{topic_count} questions</span
				>
				<Content>{info.slice(0, 100)}</Content>
				<a
					class="anchor"
					style="margin-left:10px;"
					href="/tags/edit/{id}/{name}">Edit</a
				>
			</Card>
		{/each}
	</div>
</div>

<style>
	@media (max-width: 720px) {
		.tags {
			width: 100%;
		}
	}
	@media (max-width: 4096px) {
		.tags {
			width: 800px;
		}
	}
</style>
