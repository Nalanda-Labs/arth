<script context="module">
	export function preload({ params }, { user }) {}
</script>

<script>
	import { onMount } from "svelte";
	import * as api from "api.js";
	import Card, { Content } from "@smui/card";
	import "../_utils.scss";
	import InfiniteLoading from "svelte-infinite-loading";

	let tags = [];
	let data = [];

	async function fetchData() {
		let last_tag = "";
		let topic_count = 0;
		if (tags.length) {
			last_tag = tags[tags.length - 1].name;
			topic_count = tags[tags.length - 1].topic_count;
		}
		let response = await api.post(`tags/`, { last_tag: last_tag, topic_count: topic_count });
		if (response.tags) {
			tags = response.tags;
		}
		data = tags;
	}
	onMount(async () => {
		await fetchData();
	});
	async function infiniteHandler({ detail: { loaded, complete } }) {
		let last_tag = "";
		let topic_count = 0;
		if (tags.length) {
			last_tag = tags[tags.length - 1].name;
			topic_count = tags[tags.length - 1].topic_count;
		}
		let response = await api.post(`tags/`, { last_tag: last_tag, topic_count: topic_count });
		if (response.tags) {
			tags = response.tags;
		}
		if (tags.length) {
			data = [...data, ...tags];
			loaded();
		} else {
			complete();
		}
	}
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
		{#each data as { id, info, name, topic_count }}
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
		<InfiniteLoading on:infinite={infiniteHandler} />
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
