<script context="module">
	export function preload({ params }) {
		let page = params.page;
		return { page };
	}
</script>

<script>
	import { onMount } from "svelte";
	import * as api from "api.js";
	import Card, { Content } from "@smui/card";
	import { LightPaginationNav } from "svelte-paginate";
	import "../../_utils.scss";

	export let page;
	let currentPage = page;
	let users = [];
	let count = 0;

	onMount(async () => {
		if (!page) {
			page = 1;
		}

		let response = await api.get(`users/${page}/`);
		if (response.users) {
			users = response.users;
			if (users.length > 0) {
				count = users[0].count;
			}
		}
	});
</script>

<svelte:head>
	<title>Users • Arth</title>
</svelte:head>

<div class="users">
	<h3>Users</h3>
	<div class="row">
		{#each users as { id, username, name, location, image_url }}
			<div class="col-md-3 col-sm-6">
				<img
					src="{image_url}?s=120"
					alt="{username}'s proile image"
					width="120px;"
				/>
				<br/>
				<a
					class="anchor"
					style="font-size: 12px;margin-left:5px"
					href="/user/{id}/{username}">{name || username}</a
				>
				<p style="font-size: 12px;margin-left:5px;margin-top:0px">{location}</p>
			</div>
		{/each}
	</div>
	<svelte:component
		this={LightPaginationNav}
		totalItems={count}
		pageSize="50"
		currentPage={currentPage}
		showStepOptions={true}
		on:setPage={(e) => (currentPage = e.detail.page)}
	/>
</div>

<style>
	@media (max-width: 720px) {
		.users {
			width: 100%;
		}
	}
	@media (max-width: 4096px) {
		.users {
			width: 800px;
		}
	}
</style>
