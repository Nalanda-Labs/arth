<script>
	import { goto, stores } from "@sapper/app";
	import * as api from "api.js";
	import { onMount } from "svelte";
	import "bytemd/dist/index.min.css";
	import "../_utils.scss";
	import Textfield from "@smui/textfield";
	import Button, { Label } from "@smui/button";
	import HelperText from "@smui/textfield/helper-text/index";
	import Swal from 'sweetalert2'
	// import gfm from "@bytemd/plugin-gfm";

	export let topic;
	export let id;

	let inProgress = false;
	let errors;
	const { session } = stores();

	async function onSubmit() {
		if ($session.user) {
			inProgress = true;
			if(topic.title < 6 || topic.title > 256) {
				Swal.fire(
					"Title should not be less than 6 or more than 256 characters."
				);
				return;
			}
			if (value.length < 20 || value.length > 100000) {
				Swal.fire(
					"Topic should not be less than 20 or more than 100000 characters."
				);
				return;
			}

			topic.body = value;

			if (topic.tagList.length < 1) {
				Swal.fire("At least one tag should be supplied.");
			}

			const response = await api.post(
				"t/create-topic/",
				{ topic },
				localStorage.getItem("jwt")
			);

			if (response.id && response.slug) {
				id = response.id;
				await goto(`/t/${response.id}/${response.slug}`);
			}

			inProgress = false;
		} else {
			Swal.fire("You are not logged in.");
		}
	}

	let Editor = null;
	let plugins = null;
	let value = "";
	let Tags = null;

	onMount(async () => {
		const bytemd = await import("bytemd");
		Tags = (await import("../_components/Tags.svelte")).default;
		Editor = bytemd.Editor;
	});

	function handleChange(e) {
		value = e.detail.value;
	}

	function handleTags(event) {
		topic.tagList = event.detail.tags;
		let re = /[a-zA-Z0-0\-\+]+/;
		for (let i = 0; i < topic.tagList.length; i++) {
			if (topic.tagList[i].length > 32) {
				Swal.fire("32 Characterx max.");
			}
		}
	}

	// function for auto-completing tags
	async function ts() {
		const response = await api.post("get-tags/", {
			tag: document.getElementById("tags").value,
		});
		if (response.tags) {
			let tags = [];
			for (let i = 0; i < response.tags.length; i++) {
				tags.push(response.tags[i]["name"]);
			}
			return tags;
		} else {
			return [];
		}
	}
</script>

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
	}
</style>

<div class="topic">
	<h3>Post your topic for discussion</h3>
	<hr />
	<form on:submit|preventDefault={onSubmit}>
		<div>
			<Textfield
				withTrailingIcon
				bind:value={topic.title}
				label="Title"
				type="text"
				minlength="6"
				maxlength="256"
				style="min-width:100%"
				input$aria-controls="helper-text-standard-c"
				input$aria-describedby="helper-text-standard-c" />
			<HelperText id="helper-text-standard-c">
				Summary of your topic
			</HelperText>
		</div>
		<svelte:component
			this={Editor}
			on:change={handleChange}
			mode="tab"
			{value}/>
		<div style="margin:30px"/>
		<svelte:component
			this={Tags}
			name={'tags'}
			on:tags={handleTags}
			addKeys={[9]}
			maxTags={5}
			allowPaste={true}
			allowDrop={true}
			splitWith={','}
			onlyUnique={true}
			removeKeys={[27, 8]}
			placeholder={'Tags, tab to complete'}
			allowBlur={true}
			disable={false}
			id={'tags'}
			minChars={3}
			autoComplete={ts} />
		<div class="b-wrapper">
			<Button variant="raised">
				<Label>Ask</Label>
			</Button>
		</div>
	</form>
</div>
