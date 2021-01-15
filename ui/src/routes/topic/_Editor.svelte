<script>
	import { goto, stores } from "@sapper/app";
	import * as api from "api.js";
	import { onMount } from "svelte";
	import "bytemd/dist/index.min.css";
	// import gfm from "@bytemd/plugin-gfm";

	export let topic;
	export let id;

	let inProgress = false;
	let errors;
	const { session } = stores();

	async function onSubmit() {
		if ($session.user) {
			inProgress = true;
			if (value.length < 20 || value.length > 100000) {
				alert(
					"Topic should not be less than 20 or more than 100000 characters."
				);
				return;
			}

			topic.body = value;

			if (topic.tagList.length < 1) {
				alert("At least one tag should be supplied.");
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
			alert("You are not logged in.");
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
				alert("32 Characterx max.");
			}
		}
	}

	// function for auto-completing tags
	function ts() {
		const response = api.post("get-tags/", { tags: document.getElementById("tags").value });
		if (response.tags) {
			return response.tags;
		} else {
			// alert("Error getting tags");
			return [];
		}
	}
</script>

<div class="editor-page">
	<div class="container page">
		<div class="row">
			<div class="col-md-8 offset-md-2 col-xs-12">
				<h3>Post your topic for discussion</h3>
				<hr />
				<form on:submit|preventDefault={onSubmit}>
					<fieldset>
						<fieldset class="form-group">
							<label
								for="title"
								class="form-label"
								style="float:left;margin-right: 10px;margin-top: 8px;"
								>Title:</label
							>
							<span style="display: block;overflow: hidden;">
								<input
									class="form-control form-control"
									style="width:100%;border-radius:0;"
									name="title"
									type="text"
									placeholder="Topic summary"
									bind:value={topic.title}
									required="true"
									minlength="10"
									maxlength="256"
								/>
							</span>
						</fieldset>

						<fieldset class="form-group" style="margin-top:20px">
							<svelte:component
								this={Editor}
								on:change={handleChange}
								{value}
							/>
						</fieldset>
						<fieldset class="form-group" style="margin-top:20px">
							<label
								for="tags"
								class="form-label"
								style="float:left;margin-right: 10px;margin-top: 8px"
								>Tags:</label
							>
							<svelte:component
								this={Tags}
								name={"tags"}
								on:tags={handleTags}
								addKeys={[9]}
								maxTags={5}
								allowPaste={true}
								allowDrop={true}
								splitWith={","}
								onlyUnique={true}
								removeKeys={[27]}
								placeholder={"Tags, tab to complete"}
								allowBlur={true}
								disable={false}
								id={"tags"}
								minChars={3}
								autoComplete={ts}
							/>
						</fieldset>
						<div class="wrapper">
							<button
								class="btn btn btn-primary pull-xs-right"
								data-action="submit"> Create Topic </button>
						</div>
					</fieldset>
				</form>
			</div>
		</div>
	</div>
</div>

<style>
	.wrapper {
		width: 100%;
		height: 100px;
		display: flex;
		align-items: center;
		justify-content: center;
	}
</style>
